#pragma once
#include "GameState.h"
#include "ChessBoard.h"

class minmax {
private:
    // 폰: 중앙 전진 장려
    /**
     * 
     */
    const int PAWN_TABLE[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {50, 50, 50, 50, 50, 50, 50, 50},
        {10, 10, 20, 30, 30, 20, 10, 10},
        {5, 5, 10, 25, 25, 10, 5, 5},
        {0, 0, 0, 20, 20, 0, 0, 0},
        {5, -5, -10, 0, 0, -10, -5, 5},
        {5, 10, 10, -20, -20, 10, 10, 5},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

    // 나이트: 중앙일수록 유리
    const int KNIGHT_TABLE[8][8] = {    
        {-50, -40, -30, -30, -30, -30, -40, -50},
        {-40, -20, 0, 0, 0, 0, -20, -40},
        {-30, 0, 10, 15, 15, 10, 0, -30},
        {-30, 5, 15, 20, 20, 15, 5, -30},
        {-30, 0, 15, 20, 20, 15, 0, -30},
        {-30, 5, 10, 15, 15, 10, 5, -30},
        {-40, -20, 0, 5, 5, 0, -20, -40},
        {-50, -40, -30, -30, -30, -30, -40, -50}
    };

    // 비숍: 대각선 장악, 구석 패널티
    const int BISHOP_TABLE[8][8] = {
        {-20, -10, -10, -10, -10, -10, -10, -20},
        {-10, 0, 0, 0, 0, 0, 0, -10},
        {-10, 0, 5, 10, 10, 5, 0, -10},
        {-10, 5, 5, 10, 10, 5, 5, -10},
        {-10, 0, 10, 10, 10, 10, 0, -10},
        {-10, 10, 10, 10, 10, 10, 10, -10},
        {-10, 5, 0, 0, 0, 0, 5, -10},
        {-20, -10, -10, -10, -10, -10, -10, -20}
    };

    // 룩: 7번 랭크, 열린 파일 선호
    const int ROOK_TABLE[8][8] = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {5, 10, 10, 10, 10, 10, 10, 5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {-5, 0, 0, 0, 0, 0, 0, -5},
        {0, 0, 0, 5, 5, 0, 0, 0}
    };

    // 퀸: 중앙 선호, 초반 너무 일찍 나오면 패널티
    const int QUEEN_TABLE[8][8] = {
        {-20, -10, -10, -5, -5, -10, -10, -20},
        {-10, 0, 0, 0, 0, 0, 0, -10},
        {-10, 0, 5, 5, 5, 5, 0, -10},
        {-5, 0, 5, 5, 5, 5, 0, -5},
        {0, 0, 5, 5, 5, 5, 0, -5},
        {-10, 5, 5, 5, 5, 5, 0, -10},
        {-10, 0, 5, 0, 0, 0, 0, -10},
        {-20, -10, -10, -5, -5, -10, -10, -20}
    };

    // 킹: 초중반엔 구석, 엔드게임엔 중앙
    const int KING_TABLE[8][8] = {
        {-30, -40, -40, -50, -50, -40, -40, -30},
        {-30, -40, -40, -50, -50, -40, -40, -30},
        {-30, -40, -40, -50, -50, -40, -40, -30},
        {-30, -40, -40, -50, -50, -40, -40, -30},
        {-20, -30, -30, -40, -40, -30, -30, -20},
        {-10, -20, -20, -20, -20, -20, -20, -10},
        {20, 20, 0, 0, 0, 0, 20, 20},
        {20, 30, 10, 0, 0, 10, 30, 20}
    };
public:
    GameState gs;
    int best;
    
    void Minmax() {
        if (gs.whiteTurn) {
            int best = -999'999'999;
        }
    }
    int evaluate(const ChessBoard& cb) {
        const int PIECE_VALUE[] = {0, 100, 320, 330, 500, 900, 20000}; // P N B R Q K

        int score = 0;
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                string p = cb.board[r][c];
                if (p == ".") continue;

                bool isWhite = isupper(p[0]);
                char pt = toupper(p[0]);

                int tableScore = 0;
                // 흑은 테이블을 뒤집어서 읽어야 함
                int tr = isWhite ? r : 7 - r;

                if (pt == 'P') tableScore = PAWN_TABLE[7 - tr][c];
                else if (pt == 'N') tableScore = KNIGHT_TABLE[7 - tr][c];
                else if (pt == 'B') tableScore = BISHOP_TABLE[7 - tr][c];
                else if (pt == 'R') tableScore = ROOK_TABLE[7 - tr][c];
                else if (pt == 'Q') tableScore = QUEEN_TABLE[7 - tr][c];
                else if (pt == 'K') tableScore = KING_TABLE[7 - tr][c];

                int pieceScore = 0;
                if (pt == 'P') pieceScore = PIECE_VALUE[0 + 1];
                else if (pt == 'N') pieceScore = PIECE_VALUE[1 + 1];
                else if (pt == 'B') pieceScore = PIECE_VALUE[2 + 1];
                else if (pt == 'R') pieceScore = PIECE_VALUE[3 + 1];
                else if (pt == 'Q') pieceScore = PIECE_VALUE[4 + 1];
                else if (pt == 'K') pieceScore = PIECE_VALUE[5 + 1];

                // 백은 양수, 흑은 음수
                score += isWhite ? (pieceScore + tableScore) : -(pieceScore + tableScore);
            }
        }
        return score; // 양수면 백 유리, 음수면 흑 유리
    }
	

};
