#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Structure for a card
typedef struct Card {
    int rank; // 1 for Ace, 2-10, 11 for Jack, 12 for Queen, 13 for King
    struct Card* next;
} Card;

// Structure for a node in the BST to store scores
typedef struct ScoreNode {
    int score;
    struct ScoreNode* left;
    struct ScoreNode* right;
} ScoreNode;

// Function to create a new card
Card* createCard(int rank) {
    Card* newCard = (Card*)malloc(sizeof(Card));
    newCard->rank = rank;
    newCard->next = NULL;
    return newCard;
}

// Function to insert a score into the BST
ScoreNode* insertScore(ScoreNode* root, int score) {
    if (root == NULL) {
        ScoreNode* newNode = (ScoreNode*)malloc(sizeof(ScoreNode));
        newNode->score = score;
        newNode->left = newNode->right = NULL;
        return newNode;
    }
    if (score < root->score)
        root->left = insertScore(root->left, score);
    else if (score > root->score)
        root->right = insertScore(root->right, score);
    return root;
}

// Function to print the BST in inorder traversal
void inorderTraversal(ScoreNode* root) {
    if (root != NULL) {
        inorderTraversal(root->left);
        printf("%d ", root->score);
        inorderTraversal(root->right);
    }
}

// Function to create a deck of cards
Card* createDeck() {
    Card* head = NULL;
    Card* temp = NULL;
    for (int i = 1; i <= 13; i++) { // Ace to King
        Card* newCard = createCard(i);
        if (head == NULL) {
            head = newCard;
            temp = head;
        } else {
            temp->next = newCard;
            temp = temp->next;
        }
    }
    return head;
}

// Function to shuffle the deck
Card* shuffleDeck(Card* head) {
    Card* current = head;
    Card* newHead = NULL;
    int len = 0;

    // Find the length of the deck
    while (current != NULL) {
        len++;
        current = current->next;
    }

    // Array to hold the pointers to cards
    Card** cardArray = (Card**)malloc(len * sizeof(Card*));
    current = head;
    for (int i = 0; i < len; i++) {
        cardArray[i] = current;
        current = current->next;
    }

    // Shuffle the array
    srand(time(0));
    for (int i = len - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card* tempCard = cardArray[i];
        cardArray[i] = cardArray[j];
        cardArray[j] = tempCard;
    }

    // Reconstruct the deck with shuffled cards
    newHead = cardArray[0];
    current = newHead;
    for (int i = 1; i < len; i++) {
        current->next = cardArray[i];
        current = current->next;
    }
    current->next = NULL;

    free(cardArray);
    return newHead;
}

// Function to get a string representation of the card
char* getCardName(int rank) {
    switch (rank) {
        case 1: return "A";
        case 11: return "J";
        case 12: return "Q";
        case 13: return "K";
        default: {
            char* rankStr = (char*)malloc(3 * sizeof(char));
            sprintf(rankStr, "%d", rank);
            return rankStr;
        }
    }
}

// Function to play the HiLo card game
void playGame(Card* deck) {
    ScoreNode* root = NULL;
    int score = 0;
    float main_balance = 100.0;
    float balance = 0.0;
    float baseMultiplier = 1.01; // Base multiplier
    float multiplier = baseMultiplier;
    Card* currentCard = deck;
    int continuePlaying = 1;

    printf("Welcome to the HiLo Card Game!\n");

    while (continuePlaying) {
        printf("Your current balance: $%.2f\n", main_balance);
        printf("Enter your bet amount: ");
        float bet;
        scanf("%f", &bet);
        
        // Check if the bet is valid
        if (bet <= 0) {
            printf("Bet must be greater than 0.\n");
            continue;
        }
        if (bet > main_balance) {
            printf("Insufficient balance. Your current balance is: $%.2f\n", main_balance);
            continue;
        }

        // Deduct the bet from the balance
        main_balance -= bet;

        // Start the game
        currentCard = shuffleDeck(deck); // Shuffle before each game
        while (currentCard != NULL && currentCard->next != NULL) {
            printf("Current card: %s\n", getCardName(currentCard->rank));
           
            // Check if the current card is A or K for 'same' option
            char prediction[10];
            if (currentCard->rank == 1) { // Ace
                printf("Will the next card be 'higher' or 'same'? ");
            } else if (currentCard->rank == 13) { // King
                printf("Will the next card be 'lower' or 'same'? ");
            } else {
                printf("Will the next card be 'higher', 'lower' or 'skip'? ");
            }
            scanf("%s", prediction);

            // Check if the user wants to skip this round
            if (strcmp(prediction, "skip") == 0) {
                printf("Round skipped. Moving to the next card.\n");
                currentCard = currentCard->next;
                continue;
            }

            Card* nextCard = currentCard->next;

            // Compare the current card with the next card
            if ((currentCard->rank == 1 && nextCard->rank == 1 && strcmp(prediction, "same") == 0) ||
                (currentCard->rank == 13 && nextCard->rank == 13 && strcmp(prediction, "same") == 0) ||
                (strcmp(prediction, "higher") == 0 && nextCard->rank > currentCard->rank) ||
                (strcmp(prediction, "lower") == 0 && nextCard->rank < currentCard->rank)) {
                score++;
                printf("Correct! Your current multiplier is: %.2fx\n", multiplier);
                printf("You win $%.2f this round!\n", bet * multiplier);
                balance = bet * multiplier;
               
                // Increase multiplier after each correct guess
                multiplier += 0.1;

                // Ask if the player wants to cash out
                printf("Do you want to cash out? (y/n): ");
                char cashOut;
                scanf(" %c", &cashOut);

                if (cashOut == 'y') {
                    printf("You cashed out with $%.2f. Your current balance is: $%.2f\n", balance, main_balance + balance);
                    main_balance += balance;
                    break;
                }
            } else {
                printf("Incorrect! The next card was: %s\n", getCardName(nextCard->rank));
                printf("You lose your bet of $%.2f this round.\n", bet);
                multiplier = baseMultiplier; // Reset multiplier on wrong guess
                balance = 0; // Reset balance
                break;
            }

            currentCard = currentCard->next;
        }

        root = insertScore(root, score);
        printf("Game over! Your final score is: %d\n", score);
        printf("Scores in BST:\n");
        inorderTraversal(root);
        printf("\n");

        printf("Do you want to play again? (y/n): ");
        char playAgain;
        scanf(" %c", &playAgain);
        if (playAgain != 'y') {
            continuePlaying = 0;
        } else {
            // Reset game variables
            score = 0;
            multiplier = baseMultiplier;
            balance = 0; // Reset balance for next game
        }
    }
}

// Main function
int main() {
    Card* deck = createDeck();
    deck = shuffleDeck(deck);
    playGame(deck);
    return 0;
}