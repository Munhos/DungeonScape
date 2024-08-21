#include <raylib.h>
#include <iostream>

float gravity = 5.0f;

float posiAnimaçãoA = 0.0f;
float posiAnimaçãoB = 0.0f;

struct Player {
    Vector2 playerPos;
    Vector2 velocity;
    bool isJumping;
    bool isFalling;

    Player() : playerPos{ 100, 300 }, velocity{ 0, 0 }, isJumping(false), isFalling(false) {};
};

struct Floor {
    Vector2 floorPos;
    Vector2 floorSize;

    void Draw() const {
        DrawRectangle(floorPos.x, floorPos.y, floorSize.x, floorSize.y, BLUE);
    }
};

bool TimerCalculate(float time) {
    static float timer = 0.0f;

    // Incrementa o timer com o tempo de cada quadro
    timer += GetFrameTime();

    // Verifica se o tempo definido foi alcançado
    if (timer >= time) {
        timer = 0.0f;  // Reseta o timer
        return true;   // Indica que o tempo foi alcançado
    }

    return false;  // Indica que o tempo ainda não foi alcançado
}

void AnimateMoveLadin(bool move) {
    if (move) {
        if (TimerCalculate(1.0f) < 0.5f) {
            posiAnimaçãoA = 50.0f;
            posiAnimaçãoB = 0.0f;
        } else {
            posiAnimaçãoA = 100.0f;
            posiAnimaçãoB = 0.0f;
        }
    } else {
        posiAnimaçãoA = 0.0f;
        posiAnimaçãoB = 0.0f;
    }
}

bool CheckColisionLadinFloor(Player& player, Floor& floor) {
    Texture2D ladinWalking = LoadTexture("ladinFrames.png");
    // Caixa de colisão do jogador
    Rectangle playerCollisionBox = {
        player.playerPos.x,
        player.playerPos.y,
        ladinWalking.width / 3,  // Dividindo por 3 supondo que há 3 frames na textura
        ladinWalking.height - 5
    };

    // Caixa de colisão do chão
    Rectangle floorCollisionBox = {
        floor.floorPos.x,
        floor.floorPos.y,
        floor.floorSize.x,
        floor.floorSize.y
    };

    // Verifica a colisão
    if (CheckCollisionRecs(playerCollisionBox, floorCollisionBox)) {

        return true;
    }

    return false;
}

int main() {
    SetTargetFPS(60);
    bool walkingReverse = false;

    const int screenWidth = 1000;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Dungeon Scape");

    // CONSTANTES
    const Color backgroundColor = RAYWHITE;
    const float walkingSpeed = 5.0f;

    // TEXTURES
    Texture2D ladinWalking = LoadTexture("ladinFrames.png");

    // ENTIDADES
    Player ladinPrincipalChar;

    Floor chao01;
    chao01.floorPos = { 200, 300 };
    chao01.floorSize = { 200, 50 };

    Floor chao02;
    chao02.floorPos = { 100, 350 };
    chao02.floorSize = { 50, 50 };
    
    Floor chao03;
    chao03.floorPos = { 400, 350 };
    chao03.floorSize = { 100, 50 };
    


    while (!WindowShouldClose()) {
        ladinPrincipalChar.playerPos.y += gravity;
        if (IsKeyDown(KEY_D)) {
            AnimateMoveLadin(true);
            ladinPrincipalChar.playerPos.x += walkingSpeed;
            walkingReverse = false;
        }
        if (IsKeyReleased(KEY_D)) {
            AnimateMoveLadin(false);
        }
        
        if (IsKeyDown(KEY_A)) {
            AnimateMoveLadin(true);
            ladinPrincipalChar.playerPos.x -= walkingSpeed;
            walkingReverse = true;
        }
        if (IsKeyReleased(KEY_A)) {
            AnimateMoveLadin(false);
        }
        
        if (IsKeyPressed(KEY_SPACE) && !ladinPrincipalChar.isFalling && !ladinPrincipalChar.isJumping) {
            ladinPrincipalChar.isJumping = true;   
        }
        
        if(ladinPrincipalChar.isJumping){
            if(!TimerCalculate(0.5f)){
                gravity = -5.0f;
                  
            }else{
                ladinPrincipalChar.isJumping = false;  
                ladinPrincipalChar.isFalling = true;
            }
        }
        
        if(ladinPrincipalChar.isFalling){
            if(!TimerCalculate(0.5f)){
                gravity = 5.0f;
                
            }else{
                ladinPrincipalChar.isFalling = false;
            }
        }
        
        if(CheckColisionLadinFloor(ladinPrincipalChar, chao03)){

            ladinPrincipalChar.playerPos.y -= 5;
        }
        
        if(CheckColisionLadinFloor(ladinPrincipalChar, chao02)){

            ladinPrincipalChar.playerPos.y -= 5;
        }
        
        if(CheckColisionLadinFloor(ladinPrincipalChar, chao01)){

            ladinPrincipalChar.playerPos.y -= 5;
        }

        

        BeginDrawing();
        ClearBackground(backgroundColor);

        // Desenhar o sprite com base na animação
        Rectangle sourceRec = { posiAnimaçãoA, posiAnimaçãoB, ladinWalking.width / 3, ladinWalking.height };
        Vector2 positionVec = { ladinPrincipalChar.playerPos.x, ladinPrincipalChar.playerPos.y };

        if (walkingReverse) {
            // Espelhar horizontalmente
            sourceRec.width = -sourceRec.width; // Inverter a largura
            positionVec.x -= sourceRec.width + 30; // Ajustar a posição horizontalmente para compensar o espelhamento
        }

        DrawTextureRec(ladinWalking, sourceRec, positionVec, RAYWHITE);
        
        chao01.Draw();
        chao02.Draw();
        chao03.Draw();
        
        EndDrawing();
    }

    UnloadTexture(ladinWalking); // Libera a textura da memória
    CloseWindow();

    return 0;
}
