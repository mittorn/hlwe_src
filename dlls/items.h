class CItem : public CBasePlayerItem
{
public:
	void	Spawn( void );

	// Respawning
	void	EXPORT Materialize( void );
	void	Respawn( float flTime );

	// Touch
	void	EXPORT ItemTouch( CBaseEntity *pOther );
	virtual	BOOL MyTouch( CBasePlayer *pOther ) { return FALSE; };

	float	m_flRespawnTime;
	float	m_flMoneyRandomRespawn;
};