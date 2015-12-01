// Zombie guts
class CGuts : public CBaseMonster
{
public:
	void Spawn( void );
	static CGuts *ShootGuts( entvars_t *pevOwner, Vector vecStart, Vector vecVelocity );
	void EXPORT ExplodeTouch( CBaseEntity *pOther );
	void Killed(entvars_t *pevAttacker, int iGib);
	int Classify(void) { return CLASS_PROJECTILE; };
};
