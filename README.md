# Dining Philosophers Problem

The Dining Philosophers problem is a classic multi-process synchronization
problem. The problem consists of five philosophers sitting at a table who do
nothing but think and eat. Between each philosopher, there is a single fork. In order to eat, a philosopher must have both forks. A problem can arise if
each philosopher grabs the fork on the right, then waits for the fork on the
left. In this case a deadlock has occurred, and all philosophers will starve.
Also, the philosophers should be fair. Each philosopher should be able to eat
as much as the rest.

## How to Run

1. Clone the repository:

    ```bash
    git clone git@github.com:ahmed-yesuf/dining-philosophers.git
    cd dining-philosophers
    ```

2. Compile the code:

    ```bash
    gcc -o dining_philosophers dining_philosophers.c
    ```

3. Run the executable:

    ```bash
    ./dining_philosophers
    ```

