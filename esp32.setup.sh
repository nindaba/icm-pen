# https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html#step-1-install-prerequisites

brew install cmake ninja dfu-util

# Install python3
brew install python3

# Install ESP-IDF
mkdir -p ~/esp
cd ~/esp
git clone --recursive https://github.com/espressif/esp-idf.git

cd ~/esp/esp-idf
./install.sh esp32

# In the terminal where you are going to use ESP-IDF, run:
. ~/esp/esp-idf/export.sh

# alias in zshrc 
alias get_idf='. ~/esp/esp-idf/export.sh'
