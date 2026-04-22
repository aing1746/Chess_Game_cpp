// MinMax.h
#pragma once
#include <vector>
#include <tuple>
#include <climits>
#include "PieceMovement.h"
#include "ChessBoard.h"
#include "GameState.h"

class MinMax {
private:
    PieceMovement mv;

    const int PAWN_TABLE[8][8] = {
         0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
         5,  5, 10, 25, 25, 10,  5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5, -5,-10,  0,  0,-10, -5,  5,
         5, 10, 10,-20,-20, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0
    };
    const int KNIGHT_TABLE[8][8] = {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50
    };
    const int BISHOP_TABLE[8][8] = {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20
    };
    const int ROOK_TABLE[8][8] = {
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0
    };
    const int QUEEN_TABLE[8][8] = {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20
    };
    const int KING_TABLE[8][8] = {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20
    };

    int evaluate(const ChessBoard& cb) {
        const int PIECE_VALUE[] = {100, 320, 330, 500, 900, 20000};
        int score = 0;

        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                string p = cb.board[r][c];
                if (p == ".") continue;

                bool isWhite = isupper(p[0]);
                char pt = toupper(p[0]);
                int tr = isWhite ? r : 7 - r;

                int pieceScore = 0, tableScore = 0;
                if      (pt == 'P') { pieceScore = PIECE_VALUE[0]; tableScore = PAWN_TABLE[7-tr][c]; }
                else if (pt == 'N') { pieceScore = PIECE_VALUE[1]; tableScore = KNIGHT_TABLE[7-tr][c]; }
                else if (pt == 'B') { pieceScore = PIECE_VALUE[2]; tableScore = BISHOP_TABLE[7-tr][c]; }
                else if (pt == 'R') { pieceScore = PIECE_VALUE[3]; tableScore = ROOK_TABLE[7-tr][c]; }
                else if (pt == 'Q') { pieceScore = PIECE_VALUE[4]; tableScore = QUEEN_TABLE[7-tr][c]; }
                else if (pt == 'K') { pieceScore = PIECE_VALUE[5]; tableScore = KING_TABLE[7-tr][c]; }

                score += isWhite ? (pieceScore + tableScore) : -(pieceScore + tableScore);
            }
        }
        return score;
    }

    vector<tuple<int,int,int,int>> generateMoves(const ChessBoard& cb, const GameState& gs, bool white) {
        vector<tuple<int,int,int,int>> moves;

        for (int fr = 0; fr < 8; fr++) {
            for (int fc = 0; fc < 8; fc++) {
                string p = cb.board[fr][fc];
                if (p == ".") continue;
                if (white && !isupper(p[0])) continue;
                if (!white && isupper(p[0])) continue;

                for (int tr = 0; tr < 8; tr++) {
                    for (int tc = 0; tc < 8; tc++) {
                        if (cb.isSameTeam(p, cb.board[tr][tc])) continue;
                        if (!mv.isLegalMove(cb, p, fr, fc, tr, tc,
                            gs.lastMoveWasDoublePawn, gs.lastPawnRow, gs.lastPawnCol)) continue;

                        // 이동 시뮬레이션 후 내 킹 체크 여부 확인
                        ChessBoard temp = cb;
                        temp.makeMove(fr, fc, tr, tc);
                        if (mv.inCheck(temp, white)) continue;

                        moves.push_back({fr, fc, tr, tc});
                    }
                }
            }
        }
        return moves;
    }

    // 미니맥스 + 알파베타 가지치기
    int minimax(ChessBoard cb, GameState gs, int depth, int alpha, int beta, bool isMaximizing) {
        // 탐색 종료 조건
        if (depth == 0) return evaluate(cb);

        auto moves = generateMoves(cb, gs, isMaximizing);

        // 이동 가능한 수가 없으면 체크메이트 or 스테일메이트
        if (moves.empty()) {
            if (mv.inCheck(cb, isMaximizing)) return isMaximizing ? -100000 : 100000; // 체크메이트
            return 0; // 스테일메이트
        }

        if (isMaximizing) {
            int maxScore = INT_MIN;
            for (auto& [fr, fc, tr, tc] : moves) {
                ChessBoard next = cb;
                next.makeMove(fr, fc, tr, tc);
                int score = minimax(next, gs, depth - 1, alpha, beta, false);
                maxScore = max(maxScore, score);
                alpha = max(alpha, score);
                if (beta <= alpha) break; // 가지치기
            }
            return maxScore;
        } else {
            int minScore = INT_MAX;
            for (auto& [fr, fc, tr, tc] : moves) {
                ChessBoard next = cb;
                next.makeMove(fr, fc, tr, tc);
                int score = minimax(next, gs, depth - 1, alpha, beta, true);
                minScore = min(minScore, score);
                beta = min(beta, score);
                if (beta <= alpha) break; // 가지치기
            }
            return minScore;
        }
    }

public:
    vector<tuple<int,int,int,int>> getGenerateMoves(const ChessBoard& cb, const GameState& gs, bool white) {
        return generateMoves(cb, gs, white);
    }
    // depth: 탐색 깊이 (3~4 권장, 높을수록 강하지만 느림)
    tuple<int,int,int,int> getBestMove(const ChessBoard& cb, const GameState& gs, bool whiteTurn, int depth = 3) {
        auto moves = generateMoves(cb, gs, whiteTurn);
        tuple<int,int,int,int> bestMove = moves[0];
        int bestScore = whiteTurn ? INT_MIN : INT_MAX;

        for (auto& [fr, fc, tr, tc] : moves) {
            ChessBoard next = cb;
            next.makeMove(fr, fc, tr, tc);
            int score = minimax(next, gs, depth - 1, INT_MIN, INT_MAX, !whiteTurn);

            if (whiteTurn && score > bestScore) { bestScore = score; bestMove = {fr, fc, tr, tc}; }
            if (!whiteTurn && score < bestScore) { bestScore = score; bestMove = {fr, fc, tr, tc}; }
        }
        return bestMove;
    }
};