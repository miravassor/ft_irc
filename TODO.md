## Server TODO

### 1. MAX_XXX variable
- MAXCHANNELS and ERR_TOOMANYCHANNELS to join for clients ? - optional in rfc1459
- MAXTARGETS and ERR_TOOMANYTARGETS for PRIVMSG and other cmd ? - rfc1459

### 2. Clean Signal Quit
- Ctrl+D signal
  'com', 'man', 'd\n'.

### 3. Buffer Overflow
- prevent buffer overflow in the `_buffer`

### 4. Names capitalization
- Probably store the uppercase version of channel/clients names for better performance

### 5. Limit user modes
As there's no mention of user modes in subject probably those two will be enough:
- i - marks a users as invisible;
- s - marks a user for receipt of server notices;

### 6. Packages organizing
- Something like
  - includes/
    - *.hpp
  - srcs/ 
    - commands/
      - *.cpp
    - *.cpp