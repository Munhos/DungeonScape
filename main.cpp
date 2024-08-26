#include <raylib.h>
#include <iostream>

const float gravity = 5.0f;

struct Player {
    Vector2 position;
    Vector2 velocity;
    bool isJumping;
    bool isFalling;
    bool isAttacking;

    Player() : position{ 0, 500 }, velocity{ 0, 0 }, isJumping(false), isFalling(false), isAttacking(false) {}
};

struct Floor {
    Vector2 position;
    Vector2 size;
    Texture2D texture;

    void Draw() const {
        DrawTexturePro(texture, {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)},
                      {position.x, position.y, size.x, size.y}, {0, 0}, 0.0f, WHITE);
    }

    Rectangle GetCollisionBox() const {
        return { position.x, position.y, size.x, size.y };
    }
};

struct Slime {
    Vector2 position;
    bool isActive;

    Slime() : position{100, 100}, isActive(true) {}
};

void KeepPlayerInBounds(Player& player, int screenWidth, int screenHeight, const Texture2D& texture) {
    if (player.position.x < 0) {
        player.position.x = 0;
    } else if (player.position.x + texture.width / 3 > screenWidth) {
        player.position.x = screenWidth - texture.width / 3;
    }

    if (player.position.y < 0) {
        player.position.y = 0;
    } else if (player.position.y + texture.height > screenHeight) {
        player.position.y = screenHeight - texture.height;
    }
}

bool CheckCollisionWithFloor(const Player& player, const Floor& floor, const Texture2D& texture) {
    Rectangle playerCollisionBox = {
        player.position.x,
        player.position.y,
        texture.width / 3,  // Dividindo por 3 supondo que há 3 frames na textura
        texture.height - 5
    };

    return CheckCollisionRecs(playerCollisionBox, floor.GetCollisionBox());
}

bool CheckCollisionWithSlime(const Player& player, const Slime& slime, const Texture2D& texture, const Texture2D& slimeTexture) {
    Rectangle playerCollisionBox = {
        player.position.x,
        player.position.y,
        texture.width / 3,
        texture.height - 5
    };

    Rectangle slimeCollisionBox = {
        slime.position.x,
        slime.position.y,
        slimeTexture.width * 0.1f,
        slimeTexture.height * 0.1f
    };

    return CheckCollisionRecs(playerCollisionBox, slimeCollisionBox);
}

void AnimateMoveLadin(float& posiAnimaçãoA, bool move) {
    static float timer = 0.0f;
    static bool firstFrame = true;

    if (move) {
        timer += GetFrameTime();

        if (firstFrame) {
            if (timer >= 0.5f) {  // Quando chega ao tempo, firstFrame fica negativo e ativa a segunda parte
                posiAnimaçãoA = 100.0f;
                timer = 0.0f;
                firstFrame = false;
            } else {
                posiAnimaçãoA = 50.0f;  // enquanto está de 0-0.4 fica a primeira animação
            }
        } else {
            if (timer >= 0.5f) {
                posiAnimaçãoA = 50.0f;
                timer = 0.0f;
                firstFrame = true;
            } else {
                posiAnimaçãoA = 100.0f;
            }
        }
    } else {
        posiAnimaçãoA = 0.0f;
        timer = 0.0f;
        firstFrame = true;
    }
}

void HandleAttack(Player& player, Slime& slime1, Slime& slime2, Slime& slime3, Texture2D& daggerLadin, Texture2D& slimeTexture, bool walkingReverse) {
    static float attackDuration = 0.3f; // Duração do ataque em segundos
    static float attackTimer = 0.0f;
    Vector2 daggerPosition = player.position;
    int daggerAngle = walkingReverse ? 45 : 220;
    float daggerScale = 0.08f;

    if (player.isAttacking) {
        attackTimer += GetFrameTime();

        // Ajusta a posição da adaga dependendo da direção do personagem
        if (walkingReverse) {
            daggerPosition.x -= 20;
            daggerPosition.y -= 60;
        } else {
            daggerPosition.x += 50;
            daggerPosition.y -= 20;
        }

        // Desenha a adaga
        DrawTextureEx(daggerLadin, {daggerPosition.x += 10, daggerPosition.y += 60}, daggerAngle, daggerScale, RAYWHITE);

        // Define o retângulo de ataque com base na posição e tamanho da adaga
        Rectangle attackBox = {
            daggerPosition.x,
            daggerPosition.y,
            daggerLadin.width * daggerScale,
            daggerLadin.height * daggerScale
        };

        // Verifica colisão com slime1
        Rectangle slime1Box = {
            slime1.position.x,
            slime1.position.y,
            slimeTexture.width * 0.1f, // Assume que o slime está sendo desenhado com escala de 0.1f
            slimeTexture.height * 0.1f
        };

        if (slime1.isActive && CheckCollisionRecs(attackBox, slime1Box)) {
            slime1.isActive = false;
        }

        // Verifica colisão com slime2
        Rectangle slime2Box = {
            slime2.position.x,
            slime2.position.y,
            slimeTexture.width * 0.1f, // Assume que o slime está sendo desenhado com escala de 0.1f
            slimeTexture.height * 0.1f
        };

        if (slime2.isActive && CheckCollisionRecs(attackBox, slime2Box)) {
            slime2.isActive = false;
        }

        // Verifica colisão com slime3
        Rectangle slime3Box = {
            slime3.position.x,
            slime3.position.y,
            slimeTexture.width * 0.1f, // Assume que o slime está sendo desenhado com escala de 0.1f
            slimeTexture.height * 0.1f
        };

        if (slime3.isActive && CheckCollisionRecs(attackBox, slime3Box)) {
            slime3.isActive = false;
        }

        // Verifica se o tempo de ataque acabou
        if (attackTimer >= attackDuration) {
            player.isAttacking = false;
            attackTimer = 0.0f;
        }
    }
}

void moveSlime(Slime& slime) {
    if (!slime.isActive) return;

    static float cont = 0.0f;
    static bool movingRight = true;

    cont += GetFrameTime();  // Acumula o tempo de frame

    if (movingRight) {
        slime.position.x += 1.0f;  // Move para a direita
        if (cont >= 5.0f) {  // Após 5 segundos, inverte a direção
            movingRight = false;
            cont = 0.0f;  // Reinicia o contador
        }
    } else {
        slime.position.x -= 1.0f;  // Move para a esquerda
        if (cont >= 5.0f) {  // Após 5 segundos, inverte a direção
            movingRight = true;
            cont = 0.0f;  // Reinicia o contador
        }
    }
}

void ResetGame(Player& player, Slime& slime1, Slime& slime2, Slime& slime3) {
    player.position = {0, 500};
    player.isJumping = false;
    player.isFalling = false;
    player.isAttacking = false;

    slime1.position = {200, 410};
    slime1.isActive = true;

    slime2.position = {450, 510};
    slime2.isActive = true;

    slime3.position = {600, 210};
    slime3.isActive = true;
}

bool CheckPlayerCollisionWithSlimes(const Player& player, const Slime& slime1, const Slime& slime2, const Slime& slime3, const Texture2D& playerTexture, const Texture2D& slimeTexture) {
    return (slime1.isActive && CheckCollisionWithSlime(player, slime1, playerTexture, slimeTexture)) ||
           (slime2.isActive && CheckCollisionWithSlime(player, slime2, playerTexture, slimeTexture)) ||
           (slime3.isActive && CheckCollisionWithSlime(player, slime3, playerTexture, slimeTexture));
}

void GameLoop(Texture2D& ladinWalking, Texture2D& daggerLadin, Texture2D& slimeTexture, Texture2D& dungeonBricksTexture, Texture2D& dungeonWallTexture, int screenWidth, int screenHeight, bool& gameStarted) {
    bool walkingReverse = false;

    // ENTIDADES
    Player ladinPrincipalChar;
    Slime slimeChar1;
    Slime slimeChar2;
    Slime slimeChar3;

    Floor floors[] = {
        {{200, 250 },{100, 30}, dungeonBricksTexture},
        {{300, 250 },{100, 30}, dungeonBricksTexture},
        {{400, 250 },{100, 30}, dungeonBricksTexture},
        {{500, 250 },{100, 30}, dungeonBricksTexture},
        {{600, 250 },{100, 30}, dungeonBricksTexture},
        {{700, 250 },{100, 30}, dungeonBricksTexture},
        {{800, 250 },{100, 30}, dungeonBricksTexture},
        {{900, 250 },{100, 30}, dungeonBricksTexture},
        {{0, 450 },{100, 30}, dungeonBricksTexture},
        {{100, 450 },{100, 30}, dungeonBricksTexture},
        {{200, 450 },{100, 30}, dungeonBricksTexture},
        {{300, 450 },{100, 30}, dungeonBricksTexture},
        {{400, 450 },{100, 30}, dungeonBricksTexture},
        {{500, 450 },{100, 30}, dungeonBricksTexture},
        {{600, 450 },{100, 30}, dungeonBricksTexture},
        {{700, 450 },{100, 30}, dungeonBricksTexture},
        {{800, 450 },{100, 30}, dungeonBricksTexture},
        {{0, 550}, {100, 30}, dungeonBricksTexture},
        {{100, 550}, {100, 30}, dungeonBricksTexture},
        {{200, 550}, {100, 30}, dungeonBricksTexture},
        {{300, 550}, {100, 30}, dungeonBricksTexture},
        {{400, 550}, {100, 30}, dungeonBricksTexture},
        {{500, 550}, {100, 30}, dungeonBricksTexture},
        {{600, 550}, {100, 30}, dungeonBricksTexture},
        {{700, 550}, {100, 30}, dungeonBricksTexture},
        {{800, 550}, {100, 30}, dungeonBricksTexture},
        {{900, 550}, {100, 30}, dungeonBricksTexture},
        {{100, 350}, {100, 30}, dungeonBricksTexture},
        {{400, 350}, {100, 30}, dungeonBricksTexture}
    };

    float posiAnimaçãoA = 0.0f;
    int initialPosition = ladinPrincipalChar.position.y;
    int jumpHeight = 90;  // Definição da altura máxima do salto

    slimeChar1.position = { 200 ,410 };
    slimeChar2.position = { 450,510 };
    slimeChar3.position = { 600,210 };

    while (!WindowShouldClose()) {
        BeginDrawing();
        // Desenhar o fundo da dungeon
        DrawTexture(dungeonWallTexture, 0, 0, WHITE);
        // Fechar o jogo ao pressionar "Esc"
        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseWindow();
            break;
        }

        KeepPlayerInBounds(ladinPrincipalChar, screenWidth, screenHeight, ladinWalking);
        ladinPrincipalChar.position.y += gravity;

        if (IsKeyDown(KEY_D)) {
            AnimateMoveLadin(posiAnimaçãoA, true);
            ladinPrincipalChar.position.x += 5.0f;
            walkingReverse = false;
        }
        if (IsKeyReleased(KEY_D)) {
            AnimateMoveLadin(posiAnimaçãoA, false);
        }

        if (IsKeyDown(KEY_A)) {
            AnimateMoveLadin(posiAnimaçãoA, true);
            ladinPrincipalChar.position.x -= 5.0f;
            walkingReverse = true;
        }
        if (IsKeyReleased(KEY_A)) {
            AnimateMoveLadin(posiAnimaçãoA, false);
        }

        if (IsKeyPressed(KEY_SPACE) && !ladinPrincipalChar.isJumping && !ladinPrincipalChar.isFalling) {
            ladinPrincipalChar.isJumping = true;
            ladinPrincipalChar.isFalling = true;
            initialPosition = ladinPrincipalChar.position.y;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            ladinPrincipalChar.isAttacking = true;
        }

        // Ataque
        HandleAttack(ladinPrincipalChar, slimeChar1, slimeChar2, slimeChar3, daggerLadin, slimeTexture, walkingReverse);

        // Verificar colisões com o chão
        for (auto& floor : floors) {
            if (CheckCollisionWithFloor(ladinPrincipalChar, floor, ladinWalking)) {
                ladinPrincipalChar.isFalling = false;

                // Colisão com o topo do bloco (pé do personagem)
                if (ladinPrincipalChar.position.y + ladinWalking.height > floor.position.y &&
                    ladinPrincipalChar.position.y < floor.position.y) {
                    ladinPrincipalChar.position.y = floor.position.y - ladinWalking.height + 5;
                    break;
                }

                // Colisão com a parte inferior do bloco (cabeça do personagem)
                if (ladinPrincipalChar.position.y < floor.position.y + floor.size.y &&
                    ladinPrincipalChar.position.y + ladinWalking.height > floor.position.y + floor.size.y) {
                    ladinPrincipalChar.position.y = floor.position.y + floor.size.y;
                    ladinPrincipalChar.isJumping = false;
                    ladinPrincipalChar.isFalling = true;
                    break;
                }
            }
        }

        if (ladinPrincipalChar.isJumping) {
            ladinPrincipalChar.position.y -= 10;  // Movimento de subida

            // Verifica se o personagem atingiu a altura máxima do salto ou colidiu com algo acima
            if (ladinPrincipalChar.position.y <= initialPosition - jumpHeight) {
                ladinPrincipalChar.isJumping = false;
                ladinPrincipalChar.isFalling = true; // Inicia a queda se o salto parar
            }
        }

        // Mover slimes
        moveSlime(slimeChar1);
        moveSlime(slimeChar2);
        moveSlime(slimeChar3);

        // Verificar colisão com slimes ativas
        if (CheckPlayerCollisionWithSlimes(ladinPrincipalChar, slimeChar1, slimeChar2, slimeChar3, ladinWalking, slimeTexture)) {
            gameStarted = false;  // Volta ao menu inicial
            ResetGame(ladinPrincipalChar, slimeChar1, slimeChar2, slimeChar3);
            break;
        }

        
        
        

        Rectangle sourceRec = { posiAnimaçãoA, 0.0f, ladinWalking.width / 3, ladinWalking.height };
        Vector2 positionVec = { ladinPrincipalChar.position.x, ladinPrincipalChar.position.y };

        if (walkingReverse) {
            sourceRec.width = -sourceRec.width;
            positionVec.x -= sourceRec.width + 55;
        }

        DrawTextureRec(ladinWalking, sourceRec, positionVec, RAYWHITE);

        if (slimeChar1.isActive) {
            DrawTextureEx(slimeTexture, slimeChar1.position, 0, 0.1f, RAYWHITE);
        }

        if (slimeChar2.isActive) {
            DrawTextureEx(slimeTexture, slimeChar2.position, 0, 0.1f, RAYWHITE);
        }
        
        if (slimeChar3.isActive) {
            DrawTextureEx(slimeTexture, slimeChar3.position, 0, 0.1f, RAYWHITE);
        }

        for (const auto& floor : floors) {
            floor.Draw();
        }

        EndDrawing();
    }
}

int main() {
    const int screenWidth = 1000;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Dungeon Scape");
    SetTargetFPS(60);

    // CONSTANTES

    // TEXTURES
    Texture2D ladinWalking = LoadTexture("ladinFrames.png");
    Texture2D daggerLadin = LoadTexture("daggerLadin.png");
    Texture2D slimeTexture = LoadTexture("slime.png");
    Texture2D dungeonBricksTexture = LoadTexture("dungeonBricksTexture.png");
    Texture2D dungeonWallTexture = LoadTexture("dungeonWallTexture.png");

    bool gameRunning = true;
    bool gameStarted = false;

    while (!WindowShouldClose() && gameRunning) {
        BeginDrawing();
        
        if (!gameStarted) {
            // Menu Inicial
            DrawTexture(dungeonWallTexture, 0, 0, WHITE); // Usar a textura como fundo
            DrawText("Dungeon Scape", screenWidth / 2 - 100, screenHeight / 2 - 60, 40, WHITE);
            DrawText("Pressione 1 para Iniciar", screenWidth / 2 - 140, screenHeight / 2, 20, WHITE);
            DrawText("Pressione 0 para Sair", screenWidth / 2 - 130, screenHeight / 2 + 30, 20, WHITE);

            if (IsKeyPressed(KEY_ONE)) {
                gameStarted = true;
            }
            if (IsKeyPressed(KEY_ZERO)) {
                gameRunning = false;
            }
        } else {
            // Inicia o jogo
            GameLoop(ladinWalking, daggerLadin, slimeTexture, dungeonBricksTexture, dungeonWallTexture, screenWidth, screenHeight, gameStarted);
        }

        EndDrawing();
    }

    UnloadTexture(ladinWalking);
    UnloadTexture(daggerLadin);
    UnloadTexture(slimeTexture);
    UnloadTexture(dungeonBricksTexture);
    UnloadTexture(dungeonWallTexture);
    CloseWindow();

    return 0;
}
