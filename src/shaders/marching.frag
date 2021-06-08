#version 330 core

in vec4 gl_FragCoord;

uniform vec2 screenSize;
uniform vec3 cameraPosition;
uniform float fov;
uniform mat4 rotationMatrix;

layout(location = 0) out vec3 color;

vec4 direction;

const int maxIterations = 500;
const float epsilon = 0.001;

// Lighting
const vec3 lightDir = normalize(vec3(1.0, -1.0, 1.0));
const float angleFocus = 0.85; // "Spread out" the specular light
const float shininess = 12.0;
const float ambient = 0.33;

vec3 getSkyColor(vec3 dir)
{
	float a = clamp(dir.y, -0.075, 1.0) * 1.25;
	float s = pow(max(dot(dir, -lightDir), 0.0), 256.0); // Higher near the sun
	return vec3(0.5, 0.5, 0.8) + a * vec3(0.25, 0.25, 0.5) + s * vec3(1.0, 1.0, 0.0); // Base color + gradient + sun color
}

//***********************
//* DISTANCE ESTIMATORS *
//***********************
// Distance estimators encoded as (color.rgb, distance)

vec4 de_sphere(vec3 pos, vec3 center, float radius, vec3 color)
{
	float dist = length(center - pos) - radius;
	return vec4(color.rgb, dist);
}

vec4 de_box(vec3 pos, vec3 size, vec3 color)
{
	vec3 q = abs(pos) - size;
	float dist = length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
	return vec4(color.rgb, dist);
}

vec4 de_plane(vec3 pos, vec3 normal, float height)
{
	vec3 color;

	if ((mod(pos.x, 1.0) <= 0.5 && mod(pos.z, 1.0) >= 0.5) || (mod(pos.x, 1.0) >= 0.5 && mod(pos.z, 1.0) <= 0.5))
		color = vec3(0.25, 0.25, 0.25);
	else
		color = vec3(0.5, 0.5, 0.5);

	float dist = dot(pos, normal) + height;
	return vec4(color.rgb, dist);
}

vec4 de_union(vec4 deA, vec4 deB)
{
	return (deA.w < deB.w) ? deA : deB;
}

vec4 de_subtract(vec4 deA, vec4 deB)
{
	return (-deA.w > deB.w) ? vec4(deA.rgb, -deA.w) : deB;
}

vec4 de_intersect(vec4 deA, vec4 deB)
{
	return (deA.w > deB.w) ? deA : deB;
}

vec4 de_smooth_union(vec4 deA, vec4 deB, float k)
{
	float h = clamp(0.5 + 0.5 * (deB.w - deA.w) / k, 0.0, 1.0);
	float dist = mix(deB.w, deA.w, h) - k * h * (1.0 - h);
	vec3 color = mix(deB.rgb, deA.rgb, h);
	return vec4(color, dist);
}

vec3 de_translate(vec3 pos, vec3 translation)
{
	return pos + translation;
}

float de(vec3 pos, inout vec3 color)
{
	// intersected box
	vec4 sphereBox = de_sphere(pos + vec3(5, 0, 5), vec3(0), 0.8, vec3(0.5));
	vec4 rawBox = de_box(pos + vec3(5, 0, 5), vec3(3, 1, 0.3), vec3(0.5));
	vec4 box = de_subtract(sphereBox, rawBox);

	vec4 sphereA = de_sphere(pos, vec3(0), 0.3, vec3(0.8, 0.6, 0.2));
	vec4 sphereB = de_sphere(pos + vec3(-1.0, 0, 0), vec3(0), 0.3, vec3(0.2, 0.6, 0.8));
	vec4 plane = de_plane(pos, vec3(0, 1, 0), 1);

	vec4 res = de_union(de_union(sphereA, plane), sphereB);
	res = de_smooth_union(res, box, 0.25);

	color = res.rgb;
	return res.w;
}

vec3 getNormal(vec3 p)
{
	vec3 c;
	return normalize(vec3(
	    de(p + vec3(epsilon, 0, 0), c) - de(p - vec3(epsilon, 0, 0), c),
	    de(p + vec3(0, epsilon, 0), c) - de(p - vec3(0, epsilon, 0), c),
	    de(p + vec3(0, 0, epsilon), c) - de(p - vec3(0, 0, epsilon), c)));
}

bool cast(vec3 p, vec3 d, inout vec3 hit, inout float complexity, inout vec3 color)
{
	float dist = 0;
	int i = 0;

	for (; i < maxIterations; i++)
	{
		vec3 pos = p + d * dist;
		float step = de(pos, color);

		if (step < epsilon)
		{
			hit = pos;
			complexity = float(i) / float(maxIterations);
			return true;
		}

		dist += step;
	}

	hit = vec3(0);
	complexity = 0;
	return false;
}

vec3 phong(vec3 pos, vec3 dir, vec3 normal, vec3 color)
{
	bool hit = false;
	float dist = epsilon * 30; // prevent errors
	float penumbra = 1.0; // How close the ray was to other geometry
	vec3 unused;
	
	for (int i = 0; i < 100; i++)
	{
		float step = de(pos + -lightDir * dist, unused);

		if (step < epsilon)
		{
			hit = true;
			break;
		}

		penumbra = min(penumbra, 32 * step / dist);

		dist += step;
	}

	penumbra = clamp(penumbra, 0.0, 1.0);

	if (hit)
	{
		color *= ambient;
		return color;
	}

	float diffuse = clamp(dot(-lightDir, normal), 0.0, 1.0) * penumbra;

	vec3 reflectDir = reflect(lightDir, normal);
	float specAngle = max(dot(reflectDir, -dir), 0.0);
	float specular = pow(specAngle * angleFocus, shininess);

	color = color * (ambient + diffuse) + vec3(specular);

	return color;
}

vec3 shade(vec3 dir, vec3 pos, vec3 objColor)
{
	vec3 normal = getNormal(pos);

	vec3 color = phong(pos, dir, normal, objColor);

	return color;
}

void getRay()
{
	vec2 xy = vec2(gl_FragCoord.xy) - screenSize / 2.0; // Position on the screen
	float z = screenSize.y / tan(fov / 2.0);            // Distance of the screen to the "eye" (focal length)
	direction = normalize(vec4(xy, -z, 0.0));           // Convert to ray vector (in screen space)
	direction = inverse(rotationMatrix) * direction;    // Transform into world space
}

void main()
{
	getRay();

	vec3 skybox = getSkyColor(direction.xyz);

	vec3 hit;
	float complexity;
	vec3 objColor;
	bool wasHit = cast(cameraPosition, direction.xyz, hit, complexity, objColor);

	if (wasHit)
		color = shade(direction.xyz, hit, objColor);
	else
		color = skybox;
}