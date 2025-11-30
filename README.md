# Spelling-Bee-Solver-Game
The program first starts by reading any command-line settings, such as if the user wants to play interactively or solve the puzzle automatically. 
Next, it loads the dictionary file, filtering out any words that are too short and storing the remaining words in the dynamic WordList structure. 
After the dictionary is ready, the hive letters are either requested from the user or randomly generated, and the single required center letter is set in the middle of the hive. 
If the play mode is active, the program enters a loop, taking user input, validating words against the hive rules, updating the score, and keeping track of a streak for the user.
If the solver mode is active, the program runs either the Brute Force algorithm or the faster Optimized Search algorithm to find all possible valid words that fit the hive's constraints. 
Finally, the program prints the full list of valid words, calculates the total possible score, and displays all game statistics.
Lastly all the dynamically allocated memory is freed.
