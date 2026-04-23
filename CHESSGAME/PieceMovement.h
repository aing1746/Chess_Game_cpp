// PieceMovement.h — 이동 규칙, 체크, 캐슬링 판단
#pragma once
#include "ChessBoard.h"
#include <cmath>

class PieceMovement {
public:
    
    bool isPathClear(const ChessBoard& cb, int fr, int fc, int tr, int tc) const {
        int dr = (tr>fr)?1:(tr<fr)?-1:0;
        int dc = (tc>fc)?1:(tc<fc)?-1:0;
        int r = fr+dr, c = fc+dc;
        while (r != tr || c != tc) {
            if (cb.board[r][c] != ".") return false;
            r += dr; c += dc;
        }
        return true;
    }

    bool isSquareAttacked(const ChessBoard& cb, int r, int c, bool byWhite) const {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                std::string p = cb.board[i][j];
                if (p == ".") continue;
                if (byWhite && !isupper(p[0])) continue;
                if (!byWhite && isupper(p[0])) continue;
                int dr = abs(r-i), dc = abs(c-j);
                char pt = toupper(p[0]);
                if (pt=='P') {
                    if (byWhite  && dr==1 && dc==1 && r>i) return true;
                    if (!byWhite && dr==1 && dc==1 && r<i) return true;
                } else if (pt=='R') { if ((dr==0||dc==0) && isPathClear(cb,i,j,r,c)) return true; }
                  else if (pt=='B') { if (dr==dc && isPathClear(cb,i,j,r,c)) return true; }
                  else if (pt=='Q') { if ((dr==0||dc==0||dr==dc) && isPathClear(cb,i,j,r,c)) return true; }
                  else if (pt=='N') { if ((dr==2&&dc==1)||(dr==1&&dc==2)) return true; }
                  else if (pt=='K') { if (dr<=1&&dc<=1) return true; }
            }
        }
        return false;
    }

    bool inCheck(const ChessBoard& cb, bool white) const {
        auto [kr, kc] = cb.findKing(white);
        return isSquareAttacked(cb, kr, kc, !white);
    }

    // enPassant 상태를 매개변수로 받아서 판단
    bool isLegalMove(const ChessBoard& cb, const std::string& piece,
                     int fr, int fc, int tr, int tc,
                     bool lastMoveWasDoublePawn, int lastPawnRow, int lastPawnCol) const {
        int dr = tr-fr, dc = tc-fc;
        int adr = abs(dr), adc = abs(dc);
        char pt = toupper(piece[0]);

        if (pt == 'P') {
            bool white = isupper(piece[0]);
            int dir = white ? 1 : -1;
            int startRow = white ? 1 : 6;
            if (dc==0 && cb.board[tr][tc]==".") {
                if (dr==dir) return true;
                if (fr==startRow && dr==2*dir && cb.board[fr+dir][fc]==".") return true;
            } else if (adc==1 && dr==dir && cb.isOpponent(piece, cb.board[tr][tc])) return true;
              else if (adc==1 && dr==dir && cb.board[tr][tc]=="." &&
                       lastMoveWasDoublePawn && tr==lastPawnRow+dir && tc==lastPawnCol) return true;
        }
        else if (pt=='R') return (dr==0||dc==0) && isPathClear(cb,fr,fc,tr,tc);
        else if (pt=='B') return (adr==adc) && isPathClear(cb,fr,fc,tr,tc);
        else if (pt=='Q') return (dr==0||dc==0||adr==adc) && isPathClear(cb,fr,fc,tr,tc);
        else if (pt=='N') return (adr==2&&adc==1)||(adr==1&&adc==2);
        else if (pt=='K') return adr<=1&&adc<=1;
        return false;
    }

    bool canCastle(const ChessBoard& cb, bool white, bool kingSide,
                   bool whiteKingMoved, bool blackKingMoved,
                   bool whiteRookMoved[2], bool blackRookMoved[2]) const {
        int row = white ? 0 : 7;
        if (inCheck(cb, white)) return false;
        bool& kingMoved = white ? const_cast<bool&>(whiteKingMoved) : const_cast<bool&>(blackKingMoved);
        // 실제로는 GameController에서 플래그를 넘겨받는 방식
        if (kingSide) {
            if (white ? whiteKingMoved : blackKingMoved) return false;
            if (white ? whiteRookMoved[1] : blackRookMoved[1]) return false;
            if (!isPathClear(cb,row,4,row,7)) return false;
            if (isSquareAttacked(cb,row,5,!white)||isSquareAttacked(cb,row,6,!white)) return false;
        } else {
            if (white ? whiteKingMoved : blackKingMoved) return false;
            if (white ? whiteRookMoved[0] : blackRookMoved[0]) return false;
            if (!isPathClear(cb,row,4,row,0)) return false;
            if (isSquareAttacked(cb,row,3,!white)||isSquareAttacked(cb,row,2,!white)) return false;
        }
        return true;
    }
};