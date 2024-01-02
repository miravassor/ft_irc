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
Could be nice to have names as it was entered when created
- Probably store the uppercase version of channel/clients names for better performance