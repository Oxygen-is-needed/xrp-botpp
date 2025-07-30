# v1.0.0 - Inital Working Order

- [x] Network Overview
    - [x] query()
        - [x] Specify method POST/GET
        - [x] Specify Data to send
        - [x] Return the data

- [x] Publish
    - [x] Telegram Interaction
        - [x] Commands
            - [x] 'start'   - start sending info
            - [x] 'stop'    - stop sending info
            - [x] 'end'     - remove all information about this chat
            - [x] 'leave'   - remove all info and leave chat
            - [x] 'help'    - print help text
        - [x] Send messages
            - [x] Take given info
            - [x] Fill in info in buffer
            - [x] Send buffer
        - [x] Config
            - [x] Send notification for messages (on/off)
            - [x] Configure Messages
    - [x] Save chats in file
        - [x] Save Data System
        - [x] Load Data System
        - [x] Snapshot Possibilities: can add save functions.
        - [x] Telegram Data
            - [x] Save
            - [x] Load

- [x] XRP Data Aquisition
    - [x] Check if new buy, and update
    - [x] Get Data
        - <https://github.com/adrianjustdoit58/xrp-transaction-tracker/>
        - <https://github.com/LimpidCrypto/xrpl-trading>
        - [x] Description of each in Publish Data struct.
        - [x] Time of transaction
        - [x] Transaction Address for XRPScan
        - [x] buyer
            - [x] buy address
            - [x] buy total coin
            - [x] buy total xrp
            - [x] buy total usd
        - [x] total marketcap
            - [x] usd
            - [x] xrp
        - [x] price rate
            - [x] coin
            - [x] usd
        - [x] amount holders
        - [x] amount trustline
    - [x] Fix wrong marketcap data

- [x] Main
    - [x] Escape
        - [x] set atexit and signals -- proper signal handling
        - [x] trigger the escape
        - [x] have method activate itself
            (if method is enabled then run load/save/escapesave functions)
    - [x] Args
        - [x] List all methods
        - [x] Set Method to change
            - [x] by number
            - [x] by name
        - [x] Publish Method Specific
            - [x] enable/disable each method `-e`|`--enable` || `-d`|`--disable`
            - [x] Change API Key
                - [x] by stdin via '-'
                - [x] disable echo when typing via stdin
                - [x] via argument
                - [x] via filename
                - [x] verify api key
        - [x] Help
        - [x] Version
        - [x] Expand Help Text
        - [x] Check for key file in local directory
        - [x] Check if no publish method was choosen
        - [x] Check if API key was provided
    - [x] Check Environment Variables: Overided by arguments
    - [x] Load Files
    - [x] Run Bot

- [x] Wrap up
    - [x] Fix Network Market cap error
    - [x] Remove API key
    - [x] Write Config Comments
    - [x] Look at TODO's in code
    - [x] Detect Unused Code
    - [x] Update/Set Version Number
    - [x] Write README.md
        - [x] add from help page
        - [x] description
        - [x] contribution
        - [x] license
        - [x] features
            - [x] looks for key in environment varibles first
            - [x] looks for key in local file
                - [x] conditions
                - [x] locations
- [x] Package and Release
    - [x] add: make release `pkg/`
        - [x] Makefile
        - [x] Update clean function
        - [x] Adjust docs/compiling/linux.md
    - [x] add: logo
    - [x] add: github Releases

# v1.1.0 - Update networking

- [ ] XRP
    - [ ] Check if multiple transactions not looked (keep data index?)
        - hold the data then come back to it on next update loop
    - [ ] Get wallet total
    - [ ] Store publish data, with date, in a file.

- [ ] Telegram
    - [ ] Improve message look
    - [ ] Method to add/start bot without using chat commands

- [ ] Wrap up
    - [ ] Look at TODO's in code
    - [ ] Detect Unused Code
    - [ ] Update/Set Version Number
    - [ ] Update README.md

# Future

- [ ] Log
    - [ ] Disable std::out logging
    - [ ] Log total errors encountered
    - [ ] Log total errors including from historical runs
    - [ ] Log total errors per log method
    - [ ] Log total errors per log method from historical data
    - [ ] Log to files
    - [ ] Ability to specify file per method
- [ ] Structure
    - [ ] Use Cmake
        - [ ] add cmake
        - [ ] reign in cmake
        - [ ] change compiling docs
- [ ] Main
    - [ ] Remove need for external telegram bot
    - [ ] Self reload, when an update is preformed
- [ ] Escape
    - [ ] Increase coverage of signals
        - https://stackoverflow.com/questions/11618112/most-important-signals-to-handle
    - [ ] Ensure safe functions called at exit
        - https://stackoverflow.com/questions/2663456/how-to-write-a-signal-handler-to-catch-sigsegv
- [ ] Publish
    - [ ] Specify new format for messages
    - [ ] Ability to put names to the addresses
- [ ] Network
    - [ ] Set errno type system when error is presented.
- [ ] XRP
    - [ ] Calculate Trustline #
    - [ ] Put it on a longer timer
        - every 10 update checks reset when ran

## sources

- https://deepwiki.com/reo7sp/tgbot-cpp
