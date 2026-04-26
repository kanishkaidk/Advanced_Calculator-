# advanced calculator

a small calculator project with:
- cpp console calculator using shunting-yard + postfix eval
- one minimal html ui for quick browser usage

## what it supports
- `+ - * / % ^`
- unary minus
- `recip(x)` and `reciprocal(x)`
- implicit multiplication like `2(3+4)`
- parentheses
- spaces are invalid in expressions

## run cpp (local)
```bash
g++ -std=c++17 Calculator.cpp -o calculator.exe
```

```powershell
.\calculator.exe
```

then type input like:
```text
4(15+2)^5/6%reciprocal(2)
```

## run html (local)
open `index.html` in browser.

## deploy on vercel
this repo is static from vercel side, so deploy is simple:
1. import repo in vercel
2. framework preset: `Other`
3. root: `./`
4. deploy

vercel will serve `index.html` directly.

## credit
idea inspired by dijkstra shunting-yard approach.
