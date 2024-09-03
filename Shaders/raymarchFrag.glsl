#version 330 core


in vec2 UV;

uniform float time;
uniform vec3 spherePosition;
uniform vec3 camView;
uniform vec3 camRight;
uniform vec3 camUp;
uniform vec3 camPos;

// raymarching constants
const int numSteps = 64;
const float MinDist = 0.001f;
const float MaxDist = 50.0f;

// shapes and objects to render
const vec3 cherryPos = vec3(0.0, 1.75, 0.0);
const float cherryRadius = 0.75;

const vec3 conePos = vec3(0.0, 4.0, 0.0);

vec3 dropletPosition;

out vec4 fragmentColour;


float smoothUnion(float sdf1, float sdf2, float k){
    float h = clamp(0.5f + 0.5f * (sdf2 - sdf1)/k, 0.0f, 1.0f);
    return mix(sdf2, sdf1, h) - k*h*(1.0f - h);
}

float sdfSphere(vec3 position, float radius){
    return length(position) - radius;
}

float sdfCone(vec3 position, vec2 angleCosines, float height){
    vec3 pos = position;
    vec2 q = height*vec2(angleCosines.x/angleCosines.y, -1.0);

    vec2 w = vec2(length(pos.xz), pos.y);
    vec2 a = w - q*clamp(dot(w, q)/dot(q, q), 0.0, 1.0);
    vec2 b = w - q*vec2(clamp(w.x/q.x, 0.0, 1.0), 1.0);

    float k = sign(q.y);
    float d = min(dot(a, a), dot(b, b));
    float s = max(k*(w.x*q.y - w.y*q.x), k*(w.y - q.y));

    return sqrt(d) * sign(s);
}

float sdfDroplet(vec3 position, vec2 angleCosines, float height, float radius){
    float sphereSDF = sdfSphere(position, radius);
    float coneSDF = sdfCone(position + vec3(0.0, -1.5*radius, 0.0), angleCosines, height);
    return smoothUnion(sphereSDF, coneSDF, 0.5f);
}

float sdfTorus(vec3 position, vec2 t){
    vec2 q = vec2(length(position.xz) - t.x, position.y);
    return length(q) - t.y;
}

float sdfIceCream(vec3 p){
    vec3 pos = p;

    float k = 3.0;
    float c = cos(k*pos.y);
    float s = sin(k*pos.y);

    mat2 m = mat2(c, -s, s, c);
    vec3 q = vec3(m*pos.xz, pos.y);
    float sdfTwist = sdfTorus(q, vec2(1.0));

    float sdfSphere = 1.0*sdfSphere(pos, 1.5);
    return smoothUnion(sdfTwist, sdfSphere, 0.5);
}

float sdfPlane(vec3 pos, vec3 normal, float h){
    return dot(pos, normal) + h;
}

vec3 normalSphere(vec3 position, vec3 sphereOrigin){
    return normalize(position - sphereOrigin);
}

float GlobalSDF(vec3 pos){
    float dropletRadius = 0.5f;
    // float coneHeight = 1.0f;
    // float coneAngle = 22.5;
    // vec2 angleCosines = vec2(sin(coneAngle), cos(coneAngle));
    // float dropletSDF = sdfDroplet(pos - dropletPosition, angleCosines, dropletRadius, dropletRadius);
   
    float icecreamSDF = sdfIceCream(pos);
    float cherrySDF = sdfSphere(pos - cherryPos, cherryRadius);
    
    
    float coneHeight = 3.5f;
    float coneAngle = 10.0;
    vec2 angleCosines = vec2(sin(coneAngle), cos(coneAngle));
    float coneSDF = sdfCone(vec3(pos.x, -pos.y, pos.z) - conePos, angleCosines, coneHeight);
    float globalSDF = min(coneSDF, icecreamSDF);
    
    // the min has to be taken last, to avoid having the cherry blend into the other shapes
    // (in the event that smooth unions are used)
    globalSDF = min(globalSDF, cherrySDF);
    return globalSDF;
}

vec3 sdfNormals(vec3 pos){
    float diff = 0.0001f;

    float dx = GlobalSDF(pos + vec3(diff, 0.0f, 0.0f)) - GlobalSDF(pos - vec3(diff, 0.0f, 0.0f));
    float dy = GlobalSDF(pos + vec3(0.0f, diff, 0.0f)) - GlobalSDF(pos - vec3(0.0f, diff, 0.0f));
    float dz = GlobalSDF(pos + vec3(0.0f, 0.0f, diff)) - GlobalSDF(pos - vec3(0.0f, 0.0f, diff));

    vec3 normal = normalize(vec3(dx, dy, dz));
    return normal;
}

float RayMarch(vec3 rayOrigin, vec3 rayDirection, float maxDist){
    float distance = 0.0f;

    for(int i = 0; i < numSteps; i++){
        vec3 currentPos = rayOrigin + rayDirection*distance;
        float distSDF = GlobalSDF(currentPos);
        if(distSDF < MinDist){
            break;
        }

        distance = distance + distSDF;
        if(distance > maxDist){
            break;
        }
    }
    return distance;
}


void main(){
    float aspect = 1280.0/720.0;
    vec2 uv = 2.0f*(UV - 0.5f);
    uv = vec2(aspect*uv.x, uv.y);
   
    vec3 background1 = vec3(0.3, 0.15, 0.15);
    vec3 background2 = vec3(0.15, 0.07, 0.07);    
    vec3 colour = clamp(UV.x*UV.y, 0.1, 1.0)*background1 + background2;

    // the chocolate colour
    // won't be used here, but chocolate drops will be added in the future
    //vec3 albedo = vec3(0.4, 0.2f, 0.0f);
    //dropletPosition = vec3(0.0, sin(time) + 0.3, 0.0);
    
    // ray marching
    vec3 rayOrigin = camPos;
    vec3 rayDirection = uv.x*camRight + uv.y*camUp + camView;
    float distance = RayMarch(rayOrigin, rayDirection, MaxDist);
    if(distance < MaxDist){
        if(sdfIceCream(rayOrigin + distance*rayDirection) <= MinDist){
            albedo = vec3(1.0);
        }
        if(sdfSphere(rayOrigin + distance*rayDirection - cherryPos, cherryRadius) <= MinDist){
            albedo = vec3(1.0, 0.0, 0.0);
        }

        // get normals with the SDF algorithm
        vec3 rayPoint = rayOrigin + distance*rayDirection;
        vec3 normal = sdfNormals(rayPoint);

        // phoung shading
        // diffuse
        vec3 lightColour = vec3(1.0f);
        vec3 lightDir = vec3(1.0, 0.5, 0.0);
        float diffuseStrength = max(0.0f, dot(normalize(lightDir), normal));
        vec3 diffuse = lightColour * albedo * diffuseStrength;

        // specular
        vec3 view = normalize(rayDirection);
        vec3 reflectDir = normalize(reflect(lightDir, normal));
        float shininess = 64.0f;
        float specularStrength = max(0.0f, dot(view, reflectDir));
        specularStrength = pow(specularStrength, shininess);
        vec3 specular = specularStrength * lightColour * albedo;

        colour = diffuse + specular;

        // ray marched shadows; a little bit like raytracing
        vec3 lightDirection = normalize(lightDir);
        float distLight = length(lightDir - rayPoint);

        vec3 shadowRayOrigin = rayPoint + normal*0.1f;
        vec3 shadowRayDirection = lightDirection;
        float shadowDist = RayMarch(shadowRayOrigin, shadowRayDirection, distLight);        
        if(shadowDist < distLight){
            colour *= vec3(0.25);
        }
    }

    fragmentColour = vec4(colour, 1.0f);
}
