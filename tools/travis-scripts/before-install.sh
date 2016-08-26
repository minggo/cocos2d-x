#!/bin/bash

# exit this script if any commmand fails
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
COCOS2DX_ROOT="$DIR"/../..
HOST_NAME=""

mkdir -p $HOME/bin
cd $HOME/bin

function install_android_ndk()
{
    # Download android ndk
    if [ "$TRAVIS_OS_NAME" = "osx" ]; then
        HOST_NAME="darwin"
    else
        HOST_NAME="linux"
    fi

    FILE_NAME=android-ndk-r10d-${HOST_NAME}-x86_64.bin

    echo "Download ${FILE_NAME} ..."
    curl -O http://dl.google.com/android/ndk/${FILE_NAME}
    sudo chmod +x ./$FILE_NAME
    echo "Decompress ${FILE_NAME} ..."
    ./$FILE_NAME > /dev/null
    # Rename ndk
    mv android-ndk-r10d android-ndk
}

function install_linux_environment()
{
    pushd $COCOS2DX_ROOT

    # update cmake
    # echo "update cmake ..."
    # # sudo apt-get install build-essential > /dev/null
    # # wget http://www.cmake.org/files/v3.2/cmake-3.2.2.tar.gz 
    # # tar xf cmake-3.2.2.tar.gz > /dev/null
    # # cd cmake-3.2.2
    # # ./configure > /dev/null
    # # make > /dev/null
    # # sudo make install
    # sudo add-apt-repository -y ppa:george-edison55/precise-backports 2>&1  > /dev/null
    # echo "apt-get update ..."
    # sudo apt-get update -q -y > /dev/null
    # echo "upgrade cmake ..."
    # sudo apt-get install cmake
    # cmake --version
    # cd ..
    # install dpes
    python download-deps.py -r=yes

    popd
}

# set up environment according os and target
function install_environement()
{
    if [ "$TRAVIS_OS_NAME" = "linux" ]; then
        if [ "$BUILD_TARGET" = "linux" ]; then
            install_linux_environment
        fi
    fi
}

# use NDK's clang to generate binding codes
install_android_ndk
install_environement



# if [ "$GEN_COCOS_FILES"x = "YES"x ]; then
#     exit 0
# elif [ "$GEN_BINDING"x = "YES"x ]; then
#     if [ "$TRAVIS_PULL_REQUEST" != "false" ]; then
#         exit 0
#     fi
#     install_android_ndk
# elif [ "$PLATFORM"x = "linux"x ]; then
#     sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
#     sudo apt-get update
#     sudo apt-get install gcc-4.7 g++-4.7
#     sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.6 60 --slave /usr/bin/g++ g++ /usr/bin/g++-4.6
#     sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.7 90 --slave /usr/bin/g++ g++ /usr/bin/g++-4.7
#     g++ --version
#     bash $COCOS2DX_ROOT/build/install-deps-linux.sh
#     install_android_ndk
# elif [ "$PLATFORM"x = "android"x ]; then
#     install_android_ndk

#     install_android_ndk
# elif [ "$PLATFORM"x = "mac-ios"x ]; then
#     if [ "$TRAVIS_PULL_REQUEST" != "false" ]; then
#         exit 0
#     fi

#     if [ "$PUSH_TO_MAC"x = "YES"x ]; then
#         cd $COCOS2DX_ROOT
#         git config user.email ${GH_EMAIL_MAC}
#         git config user.name ${GH_USER_MAC}
#         git checkout -B $TRAVIS_BRANCH
#         cp tools/travis-scripts/travis_mac.yml ./.travis.yml
#         git add .travis.yml
#         cat .travis.yml
#         git commit --amend -m "`git log -1 --pretty=%B`"
#         git remote add travis-mac https://$GH_USER_MAC:$GH_PASSWORD_MAC@github.com/cocos-travis-mac/cocos2d-x.git
#         git push -f travis-mac $TRAVIS_BRANCH 2> /dev/null > /dev/null
#     else
#         echo "xctool version: `xctool --version`"
#         install_android_ndk
#     fi
# else
#     echo "Unknown \$PLATFORM: '$PLATFORM'"
#     exit 1
# fi
