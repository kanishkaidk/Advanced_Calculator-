credits - https://medium.com/@raunakyadav7888/how-i-built-a-better-calculator-using-the-shunting-yard-algorithm-b359b8826045
Edsger W. Dijkstra’s Shunting Yard Algorithm
Invented by the legendary Edsger W. Dijkstra, the Shunting Yard Algorithm is a stack-based method to convert infix expressions (human-readable, e.g., 2 + 3 * 4) into postfix notation, also known as Reverse Polish Notation (RPN) (e.g., 2 3 4 * +).

This conversion is crucial because RPN expressions can be evaluated directly using a simple stack without the need for complex rules about operator precedence and parentheses. The algorithm is named because its operation resembles the tracks and switches of a railroad shunting yard.

here is the image of railroad shunting yard (you probably don’t know about it or saw it. “touch some grass man”) :

Press enter or click to view image in full size
Railroad Shunting Yard
Why Postfix Makes Life Easier
Infix expressions are great for humans: we can naturally read 2 + 3 * 4. But computers? They don’t inherently know:

Multiplication comes before addition
Parentheses group expressions
Unary minus needs special treatment
Postfix solves all of that because the order of operations is already baked into the sequence:

2 3 4 * +
Evaluating this is simple:

Push numbers onto a stack
When you see an operator, pop the required number of operands, perform the operation, and push the result back
Repeat until done
Press enter or click to view image in full size

No parentheses, no confusion, no screaming at your computer — just pure, stack-based elegance.

How the Algorithm Works: Step by Step
Here’s the Shunting Yard Algorithm:

Input: Get the equation string from the user (e.g., 2+3*4))
Tokenize: Convert the string into a list of tokens:
[2, '+', 3, '*', 4]
Bonus: detect implicit multiplication (like 2+3*4)
3. Convert to Postfix (RPN): Use the Shunting Yard logic:

Use a stack to temporarily hold operators
Handle parentheses by pushing ( and popping until )
Respect operator precedence (multiplication/division > addition/subtraction)
Respect associativity (left-to-right vs right-to-left)
Example conversion:
Press enter or click to view image in full size

Evaluate Postfix: Use a stack to compute the result step by step:

Push numbers onto the stack
When an operator is encountered, pop operands, compute, and push result
End result is the final answer
Press enter or click to view image in full size

So this is how this algorithm works.

Tokenization: More Important Than You Think
You might think: “Isn’t tokenization just splitting by spaces?”

Get Raunakyadav’s stories in your inbox
Join Medium for free to get updates from this writer.

Subscribe

Remember me for faster sign in

Ha! No.

Tokenization is the art of converting a string like:

22(-3 + 5) + recip(4 - 2)
into a clean, usable list of tokens:

[22, '*', '(', -3, '+', 5, ')', '+', 'recip', '(', 4, '-', 2, ')']
Key things tokenization must handle:

Numbers: integers, floats (can be multiple digits)
Operators: +, -, *, /, ^
Parentheses: ( and )
Unary operators: -3 vs 3-2
Implicit multiplication: 2(3+5) → insert *
Functions: recip(), sin(), etc.
Without careful tokenization, the algorithm is lost before it even begins.

now explanation over lets head to the implentation part

Coding
Strategy Recap Before Coding
So here’s the master plan for our smart calculator:

Tokenize the expression → get a clean, processable list of numbers, operators, and functions.
Convert tokens to postfix (RPN) → use the Shunting Yard Algorithm to respect precedence, parentheses, and unary operators.
Evaluate postfix using a stack → compute the result with simple push/pop operations.
Think of it as three layers of intelligence:

Layer 1 (Tokenizer): Understand what the user typed
Layer 2 (Shunting Yard): Understand how the math should flow
Layer 3 (Evaluator): Compute the answer without drama
