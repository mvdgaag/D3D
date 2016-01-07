Gaag Engine

systems			: singletons, orchestrate certain parts of the engine		
systems live in static memory and update, own and/or handle the objects, components and resources
(input, ai, rendering, time, level, resources, etc)

math and utils	: global functions and some basic vector and matrix classes
math and utils do not own data, they might rely on the resources system for specific data 
(compute shaders, noise textures, etc)

objects			: can be grouped/parented, represent things in the game		
objects are handled by the resources system
(drawable, abstract, particle system, etc)

components		: coupled to object, add specific functionality				
components are handled by their respective systems and owned by their parent object
(meshcomponent, physicscomponent, aicomponent, inputcomponent, materialcomponent, etc)

resources		: data on cpu or gpu, can be shared							
resources are used by any system, object or component, but owned by the resources system
(mesh, texture, shader)
