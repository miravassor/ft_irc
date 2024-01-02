## Server TODO

### 1. MAX_XXX variable
- MAXCHANNELS and ERR_TOOMANYCHANNELS to join for clients ? - optional in rfc1459
- MAXTARGETS and ERR_TOOMANYTARGETS for PRIVMSG and other cmd ? - optional in rfc1459
@ Maria
 
### 3. Buffer Overflow
- prevent buffer overflow in the `_buffer`

### 4. Names capitalization
- Probably store the uppercase version of channel/clients names for better performance

### 5. Limit user modes
As there's no mention of user modes in subject probably those two will be enough:
- i - marks a users as invisible;
- s - marks a user for receipt of server notices;
- gerer le mode passer en argument de la commande user
- gerer les mode user / channel 
- REWORK USER COMMAND
@ alexandre
### List des users d'un chan 
- ajouter @ aux op 
@ Maria 
###  boucle poll 
- corriger le send data pour correspondre au demande de l'eval 
@ Julien 

### CAP LS 
- revoir le fonctionnement de CAP 

### ~~PRIVMSG~~ done
~~- debugger 
@ Maria~~

### Commande NICK 
- a implementer 
@ alexandre 
### PING
- verifier comporterment (print du pong)
### WHOIS
- implement WHOIS command

#### Print de dbug
- penser a virer tous les print des debug 
- 
### Boucle du main
- eviter de quitter la boucle principale en cas d exception 

### Fcntl 
- corriger fcntl
- @ Julien 

### Kill du client
- verifier comportement en cas de kill du client

### Server password 
- Implementation password server

### ERR_UNKNOWNMODE
- return garbage char into server response

### 6. Packages organizing
- Something like
  - includes/
    - *.hpp
  - srcs/ 
    - commands/
      - *.cpp
    - *.cpp