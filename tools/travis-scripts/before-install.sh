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
    bash $COCOS2DX_ROOT/build/install-deps-linux.sh
}

function download_deps()
{
    # install dpes
    pushd $COCOS2DX_ROOT
    python download-deps.py -r=yes
    popd
}

function install_android_environment()
{
    sudo apt-get install ant -y
    ant_path=`which ant`
    ant_root=${ant_path::-4}
    echo "ant_root is ${ant_root}"
    pushd $COCOS2DX_ROOT
    python setup.py -n $HOME/bin/android-ndk -a /usr/local/android-sdk -t $ant_root
    source /home/travis/.bashrc
}

# set up environment according os and target
function install_environement_for_pull_request()
{
    # use NDK's clang to generate binding codes
    install_android_ndk
    download_deps

    if [ "$TRAVIS_OS_NAME" = "linux" ]; then
        if [ "$BUILD_TARGET" = "linux" ]; then
            install_linux_environment
        fi

        if [ "$BUILD_TARGET" = "android" ]; then
            install_android_environment
        fi
    fi
}

# should generate binding codes & cocos_files.json after merging
function install_environement_for_after_merge()
{
    install_android_ndk
    download_deps
}

# build pull request
if [ "$TRAVIS_PULL_REQUEST" != "false" ]; then
    install_environement_for_pull_request
fi

# run after merging
# - make cocos robot to send PR to cocos2d-x for new binding codes
# - generate cocos_files.json for template
if [ "$TRAVIS_PULL_REQUEST" == "false" ]; then
    install_environement_for_after_merge
fi
