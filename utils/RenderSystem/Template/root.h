$$IF(PARTSYSTEM)
#ifndef PS$$ROOT$$_H
#define PS$$ROOT$$_H
$$ELSE
#ifndef RS$$ROOT$$_H
#define RS$$ROOT$$_H
$$ENDIF

$$IF(PARTSYSTEM)
class C$$PREFIX$$$$Safe_root$$ : public CPartSystem
$$ELSE
$$IF(ROTSYSTEM)
class C$$PREFIX$$$$Safe_root$$ : public CRotatingSystem
$$ELSE
class C$$PREFIX$$$$Safe_root$$ : public CRenderSystem
$$ENDIF
$$ENDIF
{
public:
	C$$PREFIX$$$$Safe_root$$(void);
	C$$PREFIX$$$$Safe_root$$(
$$IF(PARTSYSTEM)
int maxParticles,
$$ENDIF
$$IF(ARG_ORIGIN)
vec3_t origin,
$$ENDIF
$$IF(ARG_ANGLES)
vec3_t angles,
$$ENDIF
$$IF(ROTSYSTEM)
vec3_t anglesdelta,
$$ENDIF
$$IF(ARG_SPRITE)
int sprindex,
$$ENDIF
$$IF(ARG_RENDERMODE)
int r_mode,
$$ENDIF
$$IF(ARG_RENDERCOLOR)
byte r, byte g, byte b,
$$ENDIF
$$IF(ARG_BRIGHTNESS)
float a, float adelta,
$$ENDIF
$$IF(ARG_SCALE)
float scale, float scaledelta,
$$ENDIF
$$IF(ARG_LIFE)
float timetolive
$$ENDIF
);
	~C$$PREFIX$$$$Safe_root$$(void);

$$IF(PARTSYSTEM)
$$IF(FN_INITIALIZEPARTICLE)
	virtual void InitializeParticle(int index);
$$ENDIF
$$ENDIF
$$IF(FN_UPDATE)
	virtual bool Update(float time, double elapsedTime);
$$ENDIF
$$IF(FN_RENDER)
	virtual void Render(void);
$$ENDIF
};

$$IF(PARTSYSTEM)
#endif // PS$$ROOT$$_H
$$ELSE
#endif // RS$$ROOT$$_H
$$ENDIF
