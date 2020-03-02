# UDP-Guessing-Game
Internet guessing game based on UDP. Able to handle 2 clients for 1 server. 3rd client's join attempt is denied by server. Turn-based game, each player has one try to guess a number. If number is out of range given by both clients, he can try again instead of losing his turn.

Packet schema:

|Operation Field|Answer Field|Session ID|Counter|Number (Guess/L)|
|---------------|------------|----------|-------|----------------|
|4 bits|4 bits|4 bits|4bits|8 bits|


Operation field:
- 0000 – gettingID = 0,
- 0001 – waitingForL = 1,
- 0010 – sendingNumber = 2,
- 0011 – gameBegin = 3,
- 0100 – playing = 4,
- 0111 – tooManyPlayers = 7,
- 1111 – gameEnd = 15.

Answer field:
- 0000 - getID = 0,
- 0001 – idAssigned = 1,
- 0010 – outOfRange = 2,
- 0011 – win = 3,
- 0100 – notYourTurn = 4,
- 0101 – YourTurn = 5,
- 0110 – sendingL = 6,
- 0111 – sendingGuess = 7,
- 1000 – sendingHigherL = 8,
- 1001 – sendingLowerL = 9,
- 1100 – lose = 12,
- 1101 – wrong = 13,
- 1111 – ACK = 15.
 
Credits:<br>
Robert Szczepański - /SomeLogic<br>
Mateusz Sierszulski - /mts-srs
