// GameController.h — 게임 루프, 입력 처리, 상태 관리
#pragma once
#include "ChessBoard.h"
#include "PieceMovement.h"
#include "minmax.h"
#include "GameState.h"
#include <string>


class GameController {
private:
    ChessBoard cb;
    PieceMovement mv;
    MinMax minmax;
    GameState gs; 

    bool hasAnyLegalMove(bool white) {
        for (int fr = 0; fr < 8; fr++) {
            for (int fc = 0; fc < 8; fc++) {
                string p = cb.board[fr][fc];
                if (p == ".") continue;
                if (white && !isupper(p[0])) continue;
                if (!white && isupper(p[0])) continue;
                for (int tr = 0; tr < 8; tr++) {
                    for (int tc = 0; tc < 8; tc++) {
                        if (cb.isSameTeam(p, cb.board[tr][tc])) continue;
                        string saveFrom = cb.board[fr][fc];
                        string saveTo   = cb.board[tr][tc];
                        cb.board[tr][tc] = p;
                        cb.board[fr][fc] = ".";
                        bool safe = !mv.inCheck(cb, white);
                        cb.board[fr][fc] = saveFrom;
                        cb.board[tr][tc] = saveTo;
                        if (safe) return true;
                    }
                }
            }
        }
        return false;
    }

    void updateCastleFlags(const string& piece, int fr, int fc) {
        if (piece == "K") gs.whiteKingMoved = true;
        if (piece == "k") gs.blackKingMoved = true;
        if (piece == "R" && fc == 0) gs.whiteRookMoved[0] = true;
        if (piece == "R" && fc == 7) gs.whiteRookMoved[1] = true;
        if (piece == "r" && fc == 0) gs.blackRookMoved[0] = true;
        if (piece == "r" && fc == 7) gs.blackRookMoved[1] = true;
    }

public:
    
    void Move_mal() {
        while (true) {
            cb.printBoard();
            
            // 흑을 AI로 설정
            if (!gs.whiteTurn) {
                auto moves = minmax.getGenerateMoves(cb, gs, false);
                if (moves.empty()) {
                    cb.printBoard();
                    if (mv.inCheck(cb, false)) cout << "black Checkmate! Game over\n";
                    else cout << "Stalemate! It's a draw\n";
                    break;
                }

                auto [fr, fc, tr, tc] = minmax.getBestMove(cb, gs, false);
                cb.makeMove(fr, fc, tr, tc);
                gs.whiteTurn = true;

                if (mv.inCheck(cb, true)) {
                    auto whiteMoves = minmax.getGenerateMoves(cb, gs, true);
                    if (whiteMoves.empty()) {
                        cb.printBoard();
                        cout << "white Checkmate! Game over\n";
                        break;
                    }
                    cout << "white check!\n";
                } else {
                    auto whiteMoves = minmax.getGenerateMoves(cb, gs, true);
                    if (whiteMoves.empty()) {
                        cb.printBoard();
                        cout << "Stalemate! It's a draw\n";
                        break;
                    }
                }
                continue;
            }
            
            // 변경 후 (a1 b3 형식)
            cout << (gs.whiteTurn ? "white" : "black") << " turn. (ex: e2 e4): ";
            string from, to;
            if (!(cin >> from >> to)) { cout << "Input error. Shutdown\n"; break; }

            // 입력 유효성 검사
            if (from.size() != 2 || to.size() != 2 ||
                from[0] < 'a' || from[0] > 'h' || from[1] < '1' || from[1] > '8' ||
                to[0]   < 'a' || to[0]   > 'h' || to[1]   < '1' || to[1]   > '8') {
                cout << "wrong enter. (ex: e2 e4)\n"; continue;
                }

            int fc = from[0] - 'a';
            int fr = from[1] - '1';
            int tc = to[0]   - 'a';
            int tr = to[1]   - '1';
            string piece = cb.board[fr][fc];
            if (piece == "." || (gs.whiteTurn && !isupper(piece[0])) || (!gs.whiteTurn && isupper(piece[0]))) {
                cout << "Invalid property selection\n"; continue;
            }

            // 캐슬링
            if (toupper(piece[0]) == 'K' && abs(fc-tc) == 2) {
                bool kingSide = (tc > fc);
                if (mv.canCastle(cb, gs.whiteTurn, kingSide,
                                 gs.whiteKingMoved, gs.blackKingMoved,
                                 gs.whiteRookMoved, gs.blackRookMoved)) {
                    int row = gs.whiteTurn ? 0 : 7;
                    if (kingSide) { cb.makeMove(row,4,row,6); cb.makeMove(row,7,row,5); }
                    else          { cb.makeMove(row,4,row,2); cb.makeMove(row,0,row,3); }
                    if (gs.whiteTurn) gs.whiteKingMoved = true; else gs.blackKingMoved = true;
                    gs.whiteTurn = !gs.whiteTurn;
                    continue;
                }
                cout << "Can't castling\n"; continue;
            }

            if (!mv.isLegalMove(cb, piece, fr, fc, tr, tc,
                                gs.lastMoveWasDoublePawn, gs.lastPawnRow, gs.lastPawnCol)) {
                cout << "Invalid movement\n"; continue;
            }

            // 앙파상 처리
            if (toupper(piece[0]) == 'P' && tc != fc && cb.board[tr][tc] == ".")
                cb.board[gs.lastPawnRow][gs.lastPawnCol] = ".";

            // 더블폰 기록
            gs.lastMoveWasDoublePawn = (toupper(piece[0]) == 'P' && abs(tr-fr) == 2);
            if (gs.lastMoveWasDoublePawn) { gs.lastPawnRow = tr; gs.lastPawnCol = tc; }

            // 캐슬링 플래그 업데이트 후 이동
            updateCastleFlags(piece, fr, fc);
            string savedFrom = cb.board[fr][fc], savedTo = cb.board[tr][tc];
            cb.makeMove(fr, fc, tr, tc);

            if (mv.inCheck(cb, gs.whiteTurn)) {
                cb.board[fr][fc] = savedFrom; cb.board[tr][tc] = savedTo;
                cout << "Illegal: puts own king in check\n";
                continue;
            }

            gs.whiteTurn = !gs.whiteTurn;

            // 체크 / 체크메이트 / 스테일메이트
            if (mv.inCheck(cb, gs.whiteTurn)) {
                if (!hasAnyLegalMove(gs.whiteTurn)) {
                    cb.printBoard();
                    cout << (gs.whiteTurn ? "white" : "black") << " Checkmate! Game over\n";
                    break;
                }
                cout << (gs.whiteTurn ? "white" : "black") << " check!\n";
            } else if (!hasAnyLegalMove(gs.whiteTurn)) {
                cb.printBoard();
                cout << "Stalemate! It's a draw\n";
                break;
            }
        }
    }
};