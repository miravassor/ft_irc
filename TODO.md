## Server TODO

### 1. MAX_XXX variable
~~- MAXCHANNELS and ERR_TOOMANYCHANNELS to join for clients ? - optional in rfc1459~~ done
- ~~MAXTARGETS and ERR_TOOMANYTARGETS for PRIVMSG and other cmd ?~~ done as limit of targets for cmd with targets
- ~~@ Maria~~
 
### 3. Buffer Overflow
- prevent buffer overflow in the `_buffer`
@ Julien 
- 
### 4. Names capitalization
- Probably store the uppercase version of channel/clients names for better performance

### 5. Limit user modes
As there's no mention of user modes in subject probably those two will be enough:
~~- i - marks a users as invisible;~~
- s - marks a user for receipt of server notices; -> pas obligatoire ?
~~- gerer le mode passer en argument de la commande user~~
- gerer les mode user / channel 
~~- REWORK USER COMMAND~~
~~@ alexandre~~
### ~~List des users d'un chan~~ done
- ~~ajouter @ aux op~~ 
- ~~gerer les invisibles~~ 
- ~~@ Maria~~ 
###  boucle poll 
- corriger le send data pour correspondre au demande de l'eval 
@ Julien 

### CAP LS 
- revoir le fonctionnement de CAP 

### ~~PRIVMSG~~ done
~~- debugger 
@ Maria~~

### ~~Commande NICK~~
~~- a implementer 
@ alexandre~~ 
### PING
- verifier comporterment (print du pong)
### WHOIS
- implement WHOIS command

@ Julien
#### Print de dbug
- penser a virer tous les print des debug 
- 
### Boucle du main
- eviter de quitter la boucle principale en cas d exception 
@ Julien
### Fcntl 
- corriger fcntl
- @ Julien 

### Kill du client
- verifier comportement en cas de kill du client 
@ Julien
- verifier comportement en cas de kill du client

### Server password 
- Implementation password server

### ERR_UNKNOWNMODE
- return garbage char into server response

### QUIT
- ~~close connection~~
- ~~@ Maria~~

### 6. Packages organizing
- Something like
  - includes/
    - *.hpp
  - srcs/ 
    - commands/
      - *.cpp
    - *.cpp