// FogShader.h

#ifndef FOG_SHADER_H_
#define FOG_SHADER_H_

class PlayerInfo;
class Point;



// Shader for drawing a "fog of war" overlay on the map.
class FogShader {
public:
	static void Init();
	static void Redraw();
	static void Draw(const Point &center, double zoom, const PlayerInfo &player);
};



#endif
