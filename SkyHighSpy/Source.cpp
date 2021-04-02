#include <vector>
#include <list>
#include <algorithm>
#include <random>
#include <cmath>
#include <list>
#include <string>
#include "Editable.h"
#define PLAY_IMPLEMENTATION
#include "Play.h" 

// This code has been deliberately "obfuscated" for the time being...
constexpr int NUM_PARTICLES{ 50 }; class GameObject { public:enum Type { INVALID = -1, NONE, PLAYER, ASTEROID, METEOR, ASTEROID_PIECE, GEM, LASER, RING, PARTICLE }; GameObject( Type type, float speed ); GameObject() = default; int GetSpriteID() const { return m_spriteID; }float GetXPos() const { return m_xPos; }float GetYPos() const { return m_yPos; }float GetAngle() const { return m_angle; }float GetSpeed() const { return m_speed; }float GetAlpha() const { return m_alpha; }float GetScale() const { return m_scale; }int GetSpriteWidth() const { return m_spriteWidth; }int GetSpriteHeight() const { return m_spriteHeight; }int GetSpriteRadius() const { return m_radius; }float GetFrameIndex() const { return m_frameIndex; }float GetTimer() const { return m_timer; }GameObject::Type GetObjectType() const { return m_objectType; }bool GetRemoveFlag() const { return m_removeFlag; }static std::list<GameObject*>& GetGameObjects() { return s_GameObjectsList; }void SetXPos( float newX ) { m_xPos = newX; }void SetYPos( float newY ) { m_yPos = newY; }void SetAngle( float newAngle ) { m_angle = newAngle; }void SetSpeed( float newSpeed ) { m_speed = newSpeed; }void SetAlpha( float newAlpha ) { m_alpha = newAlpha; }void SetScale( float newScale ) { m_scale = newScale; }void SetSpriteID( int newSpriteID ) { m_spriteID = newSpriteID; }void SetFrameIndex( float newframeIndex ) { m_frameIndex = newframeIndex; }void SetTimer( float newTime ) { m_timer = newTime; }void SetSpriteWidth( int newWidth ) { m_spriteWidth = newWidth; }void SetSpriteHeight( int newHeight ) { m_spriteHeight = newHeight; }void SetSpriteRadius( int newRadius ) { m_radius = newRadius; }void SetObjectType( GameObject::Type newType ) { m_objectType = newType; }void SetRemoveFlag( bool newFlag ) { m_removeFlag = newFlag; }void MoveForwards(); void WrapObject(); void UpdateObject(); void DrawObject(); bool IsOutOfWindow( int modifier = 0 ); GameObject::Type HandleLaserCollision(); std::list<GameObject*> GetGameObjectsOfType( GameObject::Type type ); protected:Type m_objectType{ GameObject::INVALID }; int m_spriteID{ 0 }; int m_spriteWidth{ 0 }; int m_spriteHeight{ 0 }; float m_xPos{ 0 }; float m_yPos{ 0 }; float m_angle{ 0.0f }; float m_speed{ 0 }; float m_scale{ 1.0f }; float m_alpha{ 1.0f }; float m_frameIndex{ 0 }; float m_animSpeed{ 1.0f }; float m_timer{ 0.0f }; bool m_removeFlag{ false }; int m_radius{ 0 }; static float s_wrapBorderSize; static std::list<GameObject*> s_GameObjectsList; }; class PlayerObject : public GameObject { public:enum State { STAND, MOVE_LEFT, MOVE_RIGHT, FLY_STRAIGHT, FLY_RIGHT, FLY_LEFT, DEAD, TEST, }; PlayerObject(); State GetPlayerState() const { return m_playerState; }void SetPlayerState( State newState ) { m_playerState = newState; }float GetEmitXPos() { return m_xPos - ( m_spriteHeight / 2.0f * sinf( m_angle ) ); }float GetEmitYPos() { return m_yPos + ( m_spriteHeight / 2.0f * cosf( m_angle ) ); }void PlayerInitialise( bool isTestState ); void UpdatePlayer(); void UpdateTestPlayer(); bool CheckCollision( GameObject* GameObject ); GameObject::Type HandleCollision(); GameObject* GetCurrentAsteroid() { return m_pCurrentAsteroid; }void LaunchPlayer(); private:State m_playerState{ STAND }; Type m_objectType{ PLAYER }; GameObject* m_pCurrentAsteroid{ nullptr }; }; class ParticleObject : public GameObject { public:ParticleObject(); static void InitialiseParticles(); static void DestroyParticles(); static void UpdateParticles( bool bEmit, float emitX, float emitY ); static void DrawParticles(); private:static std::list<ParticleObject*> s_ParticleObjectsList; }; void StartGame(); void StartLevel(); void UpdateLevel(); void DrawLevel(); void DrawStringCentred( int fontSize, float x, float y, const std::string& text ); void DestroyAsteroid( GameObject* p_Object ); void CreateAsteroidPieces( float x, float y ); void CreateRings( float x, float y ); void CreateLaser( float x, float y, float angle, float speed ); void CreateGem( float x, float y ); bool HandleKeyPress(); void ClearLevel(); void EndGame(); struct GameState { PlayerObject* pPlayer{ nullptr }; int level{ 1 }; float levelTime{ 0 }; int gems{ 0 }; }; GameState g_gameState; constexpr int DISPLAY_WIDTH{ 1280 }; constexpr int DISPLAY_HEIGHT{ 720 }; constexpr int DISPLAY_SCALE{ 1 }; constexpr int GEM_SPAWN_BORDER{ 50 }; enum { FONT_SMALL = 0, FONT_MEDIUM, FONT_LARGE }; using namespace std; void MainGameEntry( PLAY_IGNORE_COMMAND_LINE ) { Play::CreateManager( DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_SCALE ); Play::LoadBackground( "Data\\Backgrounds\\background.png" ); Play::StartAudioLoop( "Music" ); g_gameState.pPlayer = new PlayerObject; ParticleObject::InitialiseParticles(); StartGame(); }bool MainGameUpdate( float elapsedTime ) { g_gameState.levelTime += elapsedTime; UpdateLevel(); DrawLevel(); return Play::KeyDown( VK_ESCAPE ); }int MainGameExit( void ) { EndGame(); Play::DestroyManager(); return PLAY_OK; }void StartGame() { g_gameState.level = 1; srand( static_cast<unsigned int>( time( 0 ) ) ); StartLevel(); }void StartLevel() { ClearLevel(); g_gameState.levelTime = 0.0f; int difficulty = g_gameState.level - 1; int numAsteroids = DefaultNumAsteroids() + 2 * difficulty; int numMeteors = DefaultNumMeteors() + difficulty; float asteroidSpeed = DefaultAsteroidSpeed(); float meteorSpeed = DefaultMeteorSpeed(); float playerSpeed = DefaultPlayerSpeed(); g_gameState.gems = numAsteroids; for( int i = 0; i < numAsteroids; i++ ) { new GameObject( GameObject::ASTEROID, asteroidSpeed ); }for( int i = 0; i < numMeteors; i++ ) { new GameObject( GameObject::METEOR, meteorSpeed ); }g_gameState.pPlayer->PlayerInitialise( IsRocketTutorial() ); g_gameState.pPlayer->SetSpeed( playerSpeed ); }void UpdateLevel() { std::list<GameObject*>::iterator it; for( it = GameObject::GetGameObjects().begin(); it != GameObject::GetGameObjects().end();) { if( ( *it )->GetRemoveFlag() == true ) { delete ( *it ); it = GameObject::GetGameObjects().erase( it ); } else { ++it; } }g_gameState.pPlayer->UpdatePlayer(); if( g_gameState.pPlayer->HandleCollision() == GameObject::Type::GEM ) { g_gameState.gems--; CreateRings( g_gameState.pPlayer->GetXPos(), g_gameState.pPlayer->GetYPos() ); }PlayerObject::State state = g_gameState.pPlayer->GetPlayerState(); bool bEmitParticles = state == PlayerObject::FLY_STRAIGHT || state == PlayerObject::FLY_LEFT || state == PlayerObject::FLY_RIGHT || state == PlayerObject::TEST; ParticleObject::UpdateParticles( bEmitParticles && g_gameState.pPlayer->GetSpeed() > 0, g_gameState.pPlayer->GetEmitXPos(), g_gameState.pPlayer->GetEmitYPos() ); bool destroyAsteroidFlag = HandleKeyPress(); for( GameObject* pGameObject : GameObject::GetGameObjects() ) { pGameObject->UpdateObject(); if( pGameObject->GetObjectType() == GameObject::LASER ) { if( pGameObject->HandleLaserCollision() == GameObject::ASTEROID ) { CreateAsteroidPieces( pGameObject->GetXPos(), pGameObject->GetYPos() ); CreateGem( pGameObject->GetXPos(), pGameObject->GetYPos() ); } } }if( destroyAsteroidFlag ) { DestroyAsteroid( g_gameState.pPlayer->GetCurrentAsteroid() ); }if( g_gameState.gems == 0 && !IsRocketTutorial() ) { g_gameState.level++; StartLevel(); } }void DestroyAsteroid( GameObject* pAsteroid ) { if( g_gameState.pPlayer->GetTimer() > 15 ) { g_gameState.pPlayer->LaunchPlayer(); CreateAsteroidPieces( pAsteroid->GetXPos(), pAsteroid->GetYPos() ); CreateGem( pAsteroid->GetXPos(), pAsteroid->GetYPos() ); pAsteroid->SetRemoveFlag( true ); Play::PlayAudio( "EXPLODE" ); } }void CreateRings( float x, float y ) { GameObject* pObj = new GameObject( GameObject::Type::RING, 0.2f ); pObj->SetScale( 0.5f ); pObj->SetXPos( x ); pObj->SetYPos( y ); pObj = new GameObject( GameObject::Type::RING, 0.2f ); pObj->SetScale( 0.0f ); pObj->SetXPos( x ); pObj->SetYPos( y ); }void DrawLevel() { Play::DrawBackground(); GameObject* pPlayerAsteroid = g_gameState.pPlayer->GetCurrentAsteroid(); for( GameObject* pGameObject : GameObject::GetGameObjects() ) { if( pGameObject != pPlayerAsteroid )pGameObject->DrawObject(); }if( pPlayerAsteroid )pPlayerAsteroid->DrawObject(); ParticleObject::DrawParticles(); g_gameState.pPlayer->DrawObject(); int dWidth = DISPLAY_WIDTH; int dHeight = DISPLAY_HEIGHT; if( !IsRocketTutorial() ) { DrawStringCentred( FONT_MEDIUM, dWidth / 2.0f, -10, "REMAINING GEMS: " + std::to_string( g_gameState.gems ) ); if( g_gameState.pPlayer->GetPlayerState() == PlayerObject::DEAD ) { DrawStringCentred( FONT_LARGE, dWidth / 2.0f, dHeight / 2.0f - 70, "GAME OVER" ); DrawStringCentred( FONT_SMALL, dWidth / 2.0f, dHeight - 66.0f, "PRESS ENTER TO PLAy AGAIN" ); } else { if( g_gameState.levelTime < 3 ) { DrawStringCentred( FONT_LARGE, dWidth / 2.0f, dHeight / 2.0f - 70, "LEVEL: " + std::to_string( g_gameState.level ) ); }DrawStringCentred( FONT_SMALL, dWidth / 2.0f, dHeight - 66.0f, "ARROW KEYS TO ROTATE AND SPACE BAR TO LAUNCH" ); } }Play::PresentPlayBuffer(); }void DrawStringCentred( int fontSize, float xPos, float yPos, const std::string& text ) { switch( fontSize ) { case FONT_SMALL:Play::DrawFontText( "font64px", text.c_str(), { xPos, yPos }, Play::CENTRE ); break; default:case FONT_MEDIUM:Play::DrawFontText( "font105px", text.c_str(), { xPos, yPos }, Play::CENTRE ); break; case FONT_LARGE:Play::DrawFontText( "font151px", text.c_str(), { xPos, yPos }, Play::CENTRE ); break; } }bool HandleKeyPress() { switch( g_gameState.pPlayer->GetPlayerState() ) { case PlayerObject::STAND:if( GetAsyncKeyState( VK_RIGHT ) ) { g_gameState.pPlayer->SetPlayerState( PlayerObject::MOVE_RIGHT ); }if( GetAsyncKeyState( VK_LEFT ) ) { g_gameState.pPlayer->SetPlayerState( PlayerObject::MOVE_LEFT ); }if( GetAsyncKeyState( VK_SPACE ) & 0x8000 ) { return true; }break; case PlayerObject::FLY_STRAIGHT:if( GetAsyncKeyState( VK_RIGHT ) ) { g_gameState.pPlayer->SetPlayerState( PlayerObject::FLY_RIGHT ); }if( GetAsyncKeyState( VK_LEFT ) ) { g_gameState.pPlayer->SetPlayerState( PlayerObject::FLY_LEFT ); }break; case PlayerObject::TEST:g_gameState.pPlayer->UpdateTestPlayer(); break; case PlayerObject::DEAD:if( GetAsyncKeyState( VK_RETURN ) ) { StartGame(); }default:break; }return false; }void CreateAsteroidPieces( float x, float y ) { float angle = 0.0f; float frameIndex = 0; for( int i = 0; i < 3; i++ ) { GameObject* p_obj = new GameObject( GameObject::ASTEROID_PIECE, 5 ); p_obj->SetXPos( x ); p_obj->SetYPos( y ); p_obj->SetAngle( angle ); p_obj->SetFrameIndex( frameIndex ); frameIndex += 1; angle -= PLAY_PI / 1.5f; } }void CreateLaser( float x, float y, float angle, float speed ) { GameObject* p_obj = new GameObject( GameObject::LASER, speed ); float offset = g_gameState.pPlayer->GetSpriteWidth() / 2.0f; p_obj->SetXPos( x + ( offset * sin( angle ) ) ); p_obj->SetYPos( y - ( offset * cos( angle ) ) ); p_obj->SetAngle( angle ); Play::PlayAudio( "LASER" ); }void CreateGem( float x, float y ) { GameObject* p_obj = new GameObject( GameObject::GEM, 0 ); p_obj->SetXPos( x ); p_obj->SetYPos( y ); while( p_obj->IsOutOfWindow( GEM_SPAWN_BORDER ) ) { p_obj->SetXPos( static_cast<float>( rand() % DISPLAY_WIDTH ) ); p_obj->SetYPos( static_cast<float>( rand() % DISPLAY_HEIGHT ) ); } }void ClearLevel() { std::list<GameObject*>::iterator it; for( it = GameObject::GetGameObjects().begin(); it != GameObject::GetGameObjects().end();) { delete ( *it ); it = GameObject::GetGameObjects().erase( it ); } }void EndGame() { std::list<GameObject*>::iterator it; for( it = GameObject::GetGameObjects().begin(); it != GameObject::GetGameObjects().end();) { delete ( *it ); it = GameObject::GetGameObjects().erase( it ); }ParticleObject::DestroyParticles(); delete g_gameState.pPlayer; }std::list<GameObject*> GameObject::s_GameObjectsList; std::list<ParticleObject*> ParticleObject::s_ParticleObjectsList; float GameObject::s_wrapBorderSize = 0; constexpr int ASTEROID_CENTRE_OFFSET( 50 ); constexpr int COLLISION_BORDER( 100 ); constexpr float ConvertToRadians( int angle ) { return angle * ( PLAY_PI / 180.0f ); }GameObject::GameObject( GameObject::Type type, float speed ) { int dWidth = Play::GetBufferWidth(); int dHeight = Play::GetBufferHeight(); switch( type ) { case ASTEROID:m_objectType = ASTEROID; m_spriteID = Play::GetSpriteId( "asteroid" ); m_spriteWidth = Play::GetSpriteWidth( m_spriteID ); m_spriteHeight = Play::GetSpriteHeight( m_spriteID ); m_xPos = static_cast<float>( rand() % ( dWidth - m_spriteWidth / 2 ) ); m_yPos = static_cast<float>( rand() % ( dHeight - m_spriteHeight / 2 ) ); m_angle = ConvertToRadians( rand() % 360 ); m_speed = speed; m_radius = ( m_spriteHeight / 2 ) - 20; m_animSpeed = 0.2f; Play::SetSpriteOrigin( m_spriteID, m_spriteWidth / 2, ( m_spriteHeight / 2 ) - 15 ); break; case METEOR:m_objectType = METEOR; m_spriteID = Play::GetSpriteId( "meteor" ); m_spriteWidth = Play::GetSpriteWidth( m_spriteID ); m_spriteHeight = Play::GetSpriteHeight( m_spriteID ); m_xPos = static_cast<float>( rand() % ( dWidth - m_spriteWidth / 2 ) ); m_yPos = static_cast<float>( rand() % ( dHeight - m_spriteHeight / 2 ) ); m_speed = speed; m_radius = ( m_spriteHeight / 2 ) - 30; m_angle = ConvertToRadians( rand() % 360 ); Play::SetSpriteOrigin( m_spriteID, m_spriteWidth / 2, ( m_spriteHeight / 2 ) - 60 ); m_animSpeed = 0.2f; break; case ASTEROID_PIECE:m_objectType = ASTEROID_PIECE; m_spriteID = Play::GetSpriteId( "asteroid_pieces" ); m_spriteWidth = Play::GetSpriteWidth( m_spriteID ); m_spriteHeight = Play::GetSpriteHeight( m_spriteID ); m_speed = 20; Play::SetSpriteOrigin( m_spriteID, m_spriteWidth / 2, m_spriteHeight / 2 ); break; case GEM:m_objectType = GEM; m_spriteID = Play::GetSpriteId( "gem" ); m_spriteWidth = Play::GetSpriteWidth( m_spriteID ); m_spriteHeight = Play::GetSpriteHeight( m_spriteID ); m_radius = ( m_spriteHeight / 2 ); Play::SetSpriteOrigin( m_spriteID, m_spriteWidth / 2, m_spriteHeight / 2 ); break; case LASER:m_objectType = LASER; m_spriteID = Play::GetSpriteId( "laser" ); m_spriteWidth = Play::GetSpriteWidth( m_spriteID ); m_spriteHeight = Play::GetSpriteHeight( m_spriteID ); m_speed = speed; m_alpha = 0.5f; Play::SetSpriteOrigin( m_spriteID, m_spriteWidth / 2, m_spriteHeight / 2 ); break; case RING:m_objectType = RING; m_spriteID = Play::GetSpriteId( "blue_ring" ); m_spriteWidth = Play::GetSpriteWidth( m_spriteID ); m_spriteHeight = Play::GetSpriteHeight( m_spriteID ); m_speed = speed; Play::SetSpriteOrigin( m_spriteID, m_spriteWidth / 2, m_spriteHeight / 2 ); break; default:PLAY_ASSERT_MSG( false, "Error: Unknown GameObject type" ); break; }if( m_spriteWidth / 2.0f > s_wrapBorderSize ) { s_wrapBorderSize = static_cast<float>( m_spriteWidth / 2.0f ); }if( m_spriteHeight / 2.0f > s_wrapBorderSize ) { s_wrapBorderSize = static_cast<float>( m_spriteHeight / 2.0f ); }s_GameObjectsList.push_back( this ); }void GameObject::MoveForwards() { m_xPos = m_xPos + m_speed * sin( m_angle ); m_yPos = m_yPos - m_speed * cos( m_angle ); }void GameObject::WrapObject() { int dWidth = Play::GetBufferWidth(); int dHeight = Play::GetBufferHeight(); if( m_xPos - s_wrapBorderSize > dWidth ) { m_xPos = 0.0f - s_wrapBorderSize; } else if( m_xPos + s_wrapBorderSize < 0 ) { m_xPos = dWidth + s_wrapBorderSize; }if( m_yPos - s_wrapBorderSize > dHeight ) { m_yPos = 0.0f - s_wrapBorderSize; } else if( m_yPos + s_wrapBorderSize < 0 ) { m_yPos = dHeight + s_wrapBorderSize; } }void GameObject::UpdateObject() { switch( GetObjectType() ) { case GameObject::ASTEROID:case GameObject::METEOR:MoveForwards(); WrapObject(); m_frameIndex += m_animSpeed; break; case GameObject::ASTEROID_PIECE:m_speed *= 0.99f; MoveForwards(); if( IsOutOfWindow() ) { m_removeFlag = true; }break; case GameObject::LASER:UpdateLaserMovement( m_xPos, m_yPos, m_angle, m_speed ); if( IsOutOfWindow() ) { m_removeFlag = true; }break; case GameObject::RING:m_alpha -= 0.05f; if( m_alpha <= 0 ) { m_removeFlag = true; m_alpha = 0; }m_scale += m_speed; break; case GameObject::GEM:m_timer++; m_angle = 0.25f * sin( m_timer / 20.0f ); break; default:break; } }GameObject::Type GameObject::HandleLaserCollision() { if( m_objectType != LASER )return GameObject::NONE; for( GameObject* pGameObject : s_GameObjectsList ) { if( pGameObject->m_objectType == ASTEROID ) { if( IsLaserCollidingWithAsteroid( m_xPos, m_yPos, pGameObject->m_xPos, pGameObject->m_yPos ) ) { pGameObject->m_removeFlag = true; m_removeFlag = true; Play::PlayAudio( "EXPLODE" ); return GameObject::ASTEROID; } } }return GameObject::NONE; }void GameObject::DrawObject() { Play::DrawSpriteRotated( m_spriteID, { m_xPos, m_yPos }, static_cast<int>( m_frameIndex ), m_angle, m_scale, m_alpha ); }bool GameObject::IsOutOfWindow( int modifier ) { int originX = static_cast<int>( Play::GetSpriteOrigin( m_spriteID ).x ); int originY = static_cast<int>( Play::GetSpriteOrigin( m_spriteID ).y ); int dWidth = static_cast<int>( Play::GetBufferWidth() ); int dHeight = static_cast<int>( Play::GetBufferHeight() ); return ( ( m_xPos - originX > dWidth - modifier ) || ( m_xPos + m_spriteWidth - originX < modifier ) || ( m_yPos - originY > dHeight - modifier ) || ( m_yPos + m_spriteHeight - originY < modifier ) ); }std::list<GameObject*> GameObject::GetGameObjectsOfType( GameObject::Type type ) { std::list<GameObject*> list; for( GameObject* pGameObject : s_GameObjectsList ) { if( pGameObject->m_objectType == type )list.push_back( pGameObject ); }return list; }PlayerObject::PlayerObject() : GameObject() { m_objectType = PLAYER; m_spriteID = Play::GetSpriteId( "agent8_left" ); m_spriteWidth = Play::GetSpriteWidth( m_spriteID ); m_spriteHeight = Play::GetSpriteHeight( m_spriteID ); m_radius = ( m_spriteHeight / 2 ) - 30; }void PlayerObject::PlayerInitialise( bool isTestState ) { int dWidth = Play::GetBufferWidth(); int dHeight = Play::GetBufferHeight(); m_timer = 0; if( isTestState ) { m_playerState = PlayerObject::TEST; m_xPos = dWidth / 2.0f; m_yPos = dHeight / 2.0f; m_angle = -PLAY_PI / 2.0f; m_pCurrentAsteroid = nullptr; } else { std::list<GameObject*> allAsteroids = GameObject::GetGameObjectsOfType( GameObject::ASTEROID ); if( allAsteroids.size() > 0 ) { GameObject* firstAsteroid = allAsteroids.front(); m_xPos = dWidth / 2.0f; m_yPos = dHeight / 2.0f; firstAsteroid->SetXPos( m_xPos ); firstAsteroid->SetYPos( m_yPos ); m_angle = 0.0f; m_pCurrentAsteroid = firstAsteroid; m_playerState = PlayerObject::STAND; } }Play::SetSpriteOrigin( Play::GetSpriteId( "agent8_left" ), m_spriteWidth / 2, ( m_spriteHeight / 2 ) + ASTEROID_CENTRE_OFFSET ); Play::SetSpriteOrigin( Play::GetSpriteId( "agent8_right" ), m_spriteWidth / 2, ( m_spriteHeight / 2 ) + ASTEROID_CENTRE_OFFSET ); Play::SetSpriteOrigin( Play::GetSpriteId( "agent8_fly" ), m_spriteWidth / 2, m_spriteHeight / 2 ); Play::SetSpriteOrigin( Play::GetSpriteId( "rocket" ), m_spriteWidth / 2, m_spriteHeight / 2 ); int deadId = Play::GetSpriteId( "agent8_dead" ); Play::SetSpriteOrigin( deadId, m_spriteWidth / 2, Play::GetSpriteHeight( deadId ) - ( m_spriteHeight / 2 ) ); }void PlayerObject::UpdatePlayer() { GameObject* currentAsteroid = GetCurrentAsteroid(); switch( m_playerState ) { case PlayerObject::STAND:if( m_spriteID != Play::GetSpriteId( "agent8_right" ) )m_spriteID = Play::GetSpriteId( "agent8_left" ); m_xPos = currentAsteroid->GetXPos(); m_yPos = currentAsteroid->GetYPos(); m_frameIndex = 0; m_animSpeed = 0; WrapObject(); break; case PlayerObject::MOVE_LEFT:m_spriteID = Play::GetSpriteId( "agent8_left" ); m_angle -= 0.05f; m_xPos = currentAsteroid->GetXPos(); m_yPos = currentAsteroid->GetYPos(); m_animSpeed = 0.5f; WrapObject(); m_playerState = PlayerObject::STAND; break; case PlayerObject::MOVE_RIGHT:m_spriteID = Play::GetSpriteId( "agent8_right" ); m_angle += 0.05f; m_xPos = currentAsteroid->GetXPos(); m_yPos = currentAsteroid->GetYPos(); m_animSpeed = 0.5f; WrapObject(); m_playerState = PlayerObject::STAND; break; case PlayerObject::FLY_STRAIGHT:m_spriteID = Play::GetSpriteId( "agent8_fly" ); MoveForwards(); WrapObject(); break; case PlayerObject::FLY_LEFT:m_spriteID = Play::GetSpriteId( "agent8_fly" ); m_angle -= 0.01f; MoveForwards(); WrapObject(); m_playerState = PlayerObject::FLY_STRAIGHT; break; case PlayerObject::FLY_RIGHT:m_spriteID = Play::GetSpriteId( "agent8_fly" ); m_angle += 0.01f; MoveForwards(); WrapObject(); m_playerState = PlayerObject::FLY_STRAIGHT; break; case PlayerObject::DEAD:m_spriteID = Play::GetSpriteId( "agent8_dead" ); m_animSpeed = 0.2f; MoveForwards(); WrapObject(); break; case PlayerObject::TEST:m_spriteID = Play::GetSpriteId( "rocket" ); m_frameIndex = 0; break; }m_frameIndex += m_animSpeed; m_timer++; }void PlayerObject::UpdateTestPlayer() { UpdatePlayerMovement( m_xPos, m_yPos, m_angle, m_speed ); WrapObject(); }void PlayerObject::LaunchPlayer() { m_pCurrentAsteroid = nullptr; m_playerState = PlayerObject::FLY_STRAIGHT; m_spriteID = Play::GetSpriteId( "agent8_fly" ); m_xPos += static_cast<int>( ASTEROID_CENTRE_OFFSET * sin( m_angle ) ); m_yPos -= static_cast<int>( ASTEROID_CENTRE_OFFSET * cos( m_angle ) ); m_timer = 0; }GameObject::Type PlayerObject::HandleCollision() { for( GameObject* pGameObject : s_GameObjectsList ) { if( CheckCollision( pGameObject ) ) { switch( m_playerState ) { case PlayerObject::FLY_STRAIGHT:case PlayerObject::FLY_LEFT:case PlayerObject::FLY_RIGHT:switch( pGameObject->GetObjectType() ) { case GameObject::ASTEROID:m_pCurrentAsteroid = pGameObject; m_playerState = PlayerObject::STAND; m_angle -= PLAY_PI; return GameObject::ASTEROID; break; case GameObject::METEOR:if( IsOutOfWindow( COLLISION_BORDER ) || pGameObject->IsOutOfWindow( COLLISION_BORDER ) )return GameObject::NONE; Play::PlayAudio( "COMBUST" ); m_playerState = PlayerObject::DEAD; return GameObject::METEOR; break; default:break; }case PlayerObject::TEST:switch( pGameObject->GetObjectType() ) { case GameObject::ASTEROID:case GameObject::METEOR:m_speed = pGameObject->GetSpeed() * 2.0f; m_angle = atan2( m_yPos - pGameObject->GetYPos(), m_xPos - pGameObject->GetXPos() ); Play::PlayAudio( "CLANG" ); return GameObject::METEOR; break; }break; default:break; }if( pGameObject->GetObjectType() == GameObject::GEM ) { if( pGameObject->GetTimer() > 5 ) { pGameObject->SetRemoveFlag( true ); Play::PlayAudio( "REWARD" ); return GameObject::GEM; } } } }return GameObject::NONE; }bool PlayerObject::CheckCollision( GameObject* obj ) { float dx = m_xPos - obj->GetXPos(); float dy = m_yPos - obj->GetYPos(); int radii = ( m_radius + ( obj->GetSpriteRadius() ) ); return ( ( dx * dx ) + ( dy * dy ) < radii * radii ); }ParticleObject::ParticleObject() : GameObject() { m_objectType = GameObject::PARTICLE; m_spriteID = Play::GetSpriteId( "particle" ); m_spriteWidth = Play::GetSpriteWidth( m_spriteID ); m_spriteHeight = Play::GetSpriteHeight( m_spriteID ); Play::SetSpriteOrigin( m_spriteID, m_spriteWidth / 2, m_spriteHeight / 2 ); s_ParticleObjectsList.push_back( this ); }void ParticleObject::DrawParticles() { for( ParticleObject* pParticle : s_ParticleObjectsList ) { Play::DrawSpriteRotated( pParticle->m_spriteID, { pParticle->m_xPos, pParticle->m_yPos }, static_cast<int>( pParticle->m_frameIndex ), 0, pParticle->m_scale, pParticle->m_alpha ); } }void ParticleObject::InitialiseParticles() { for( int i = 0; i < NUM_PARTICLES; i++ ) { new ParticleObject(); } }void ParticleObject::DestroyParticles() { std::list<ParticleObject*>::iterator i; for( i = s_ParticleObjectsList.begin(); i != s_ParticleObjectsList.end();) { delete ( *i ); i = s_ParticleObjectsList.erase( i ); } }void ParticleObject::UpdateParticles( bool bEmit, float emitX, float emitY ) { ParticleObject* pOldest = s_ParticleObjectsList.front(); for( ParticleObject* pParticle : s_ParticleObjectsList ) { if( pParticle->m_timer > 0 ) { pParticle->m_timer -= 50.0f / NUM_PARTICLES; }pParticle->m_alpha = pParticle->m_timer / 100.0f; pParticle->m_scale = pParticle->m_timer / 50.0f; if( pParticle->m_timer < pOldest->m_timer )pOldest = pParticle; }if( bEmit && pOldest->m_timer <= 0 ) { pOldest->m_xPos = emitX + rand() % 30 - 15; pOldest->m_yPos = emitY + rand() % 30 - 15; pOldest->m_timer = NUM_PARTICLES; pOldest->m_alpha = 0.5f; } }