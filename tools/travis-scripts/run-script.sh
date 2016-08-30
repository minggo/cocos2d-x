#!/bin/bash

# exit this script if any commmand fails
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
COCOS2DX_ROOT="$DIR"/../..

function build_linux()
{
    echo "Building tests ..."
    cd $COCOS2DX_ROOT/build
    mkdir -p linux-build
    cd linux-build
    cmake ../..
    make -j10
}

function build_android()
{
    # Build all samples
    echo "Building Android samples ..."
    export COCOS_CONSOLE_ROOT=$COCOS2DX_ROOT/tools/cocos2d-console/bin
    export ANT_ROOT=/usr/bin
    export ANDROID_SDK_ROOT=/usr/local/android-sdk
    export COCOS_X_ROOT=$COCOS2DX_ROOT
    export PATH=$ANT_ROOT:$ANDROID_SDK_ROOT:$COCOS_CONSOLE_ROOT:$PATH
    echo "PATH is $PATH"
    cd $COCOS2DX_ROOT/build
    ./android-build.py all
}

function genernate_binding_codes()
{
    # set environment variables needed by binding codes

    if [ -z "$NDK_ROOT" ]; then
        export NDK_ROOT=$HOME/bin/android-ndk
    fi

    if [ -z "$PYTHON_BIN" ]; then
        export PYTHON_BIN=/usr/bin/python
    fi

    # Generate binding glue codes

    echo "Create auto-generated luabinding glue codes."
    pushd "$COCOS2DX_ROOT/tools/tolua"
    ./genbindings.py
    popd

    echo "Create auto-generated jsbinding glue codes."
    pushd "$COCOS2DX_ROOT/tools/tojs"
    ./genbindings.py
    popd
}

function generate_pull_request_for_binding_codes_and_cocosfiles()
{
    COCOS_ROBOT_REMOTE="https://${GH_USER}:${GH_PASSWORD}@github.com/${GH_USER}/cocos2d-x.git"
    LUA_AUTO_GENERATE_SCRIPT_PATH="$COCOS2DX_ROOT/cocos/scripting/lua-bindings/auto"
    JS_AUTO_GENERATE_SCRIPT_PATH="$COCOS2DX_ROOT/cocos/scripting/js-bindings/auto"
    COCOSFILE_PATH="$COCOS2DX_ROOT/templates/cocos2dx_files.json"
    ELAPSEDSECS=`date +%s`
    COCOS_BRANCH="update_lua_bindings_$ELAPSEDSECS"
    COMMITTAG="[ci skip][AUTO]: updating luabinding & jsbinding & cocos_file.json automatically"
    PULL_REQUEST_REPO="https://api.github.com/repos/cocos2d/cocos2d-x/pulls"

    pushd "$COCOS2DX_ROOT"
    #Set git user for cocos2d-lua repo
    git config user.email ${GH_EMAIL}
    git config user.name ${GH_USER}#Set remotes
    git remote add upstream "$COCOS_ROBOT_REMOTE" 2> /dev/null > /dev/null

    # Run status to record the output in the log
    git status

    echo
    echo Comparing with origin HEAD ...
    echo
    git fetch origin "$TRAVIS_BRANCH"

    # Don't exit on non-zero return value
    set +e
    git diff FETCH_HEAD --stat --exit-code "$LUA_AUTO_GENERATE_SCRIPT_PATH"
    LUA_DIFF_RETVAL=$?

    git diff FETCH_HEAD --stat --exit-code "$JS_AUTO_GENERATE_SCRIPT_PATH"
    JS_DIFF_RETVAL=$?

    # generate cocos_files.json and check diff
    echo "Updates cocos_files.json"
    ./generate-template-files.py
    git diff FETCH_HEAD --stat --exit-code "$COCOSFILE_PATH"
    COCOSFILE_DIFF_RETVAL=$?

    if [ $LUA_DIFF_RETVAL -eq 0 && $JS_DIFF_RETVAL -eq 0 && $COCOSFILE_DIFF_RETVAL -eq 0 ]; then
        echo
        echo "No differences in generated files"
        echo "Exiting with success."
        echo
        exit 0
    else
        echo
        echo "Generated files differ from HEAD. Continuing."
        echo
    fi

    # Exit on error
    set -e

    git add -f --all "$LUA_AUTO_GENERATE_SCRIPT_PATH"
    git add -f --all "$JS_AUTO_GENERATE_SCRIPT_PATH"
    git add -f --all "$COCOSFILE_PATH"
    git checkout -b "$COCOS_BRANCH"
    git commit -m "$COMMITTAG"

    echo "Pushing to Robot's repo ..."
    git push -fq upstream "$COCOS_BRANCH" 2> /dev/null

    echo "Sending Pull Request to base repo ..."
    curl --user "${GH_USER}:${GH_PASSWORD}" --request POST --data "{ \"title\": \"$COMMITTAG\", \"body\": \"\", \"head\": \"${GH_USER}:${COCOS_BRANCH}\", \"base\": \"${TRAVIS_BRANCH}\"}" "${PULL_REQUEST_REPO}" 2> /dev/null > /dev/null

    popd
}

function run_pull_request()
{
    # need to generate binding codes for all targets
    genernate_binding_codes

    if [ $BUILD_TARGET == 'linux' ]; then
        build_linux
    fi

    if [ $BUILD_TARGET == 'android' ]; then
        build_android
    fi

    if [ $BUILD_TARGET == 'mac' ]; then
        cd $COCOS2DX_ROOT
        xctool -project build/cocos2d_tests.xcodeproj -scheme "build all tests Mac" -jobs 8 -arch x86_64 -sdk iphonesimulator9.3  build
    fi

    if [ $BUILD_TARGET == 'ios' ]; then
        cd $COCOS2DX_ROOT
        xctool -project build/cocos2d_tests.xcodeproj -scheme "build all tests iOS" -jobs 8 -arch i386 -sdk iphonesimulator9.3  build
    fi
}

function run_after_merge()
{
    # Re-generation of the javascript bindings can perform push of the new
    # version back to github.  We don't do this for pull requests, or if
    # GH_USER/GH_EMAIL/GH_PASSWORD environment variables are not set correctly
    # by the encoded variables in the .travis.yml file.  (e.g. if cloned repo's
    # want to use travis).
    if [ -z "${GH_EMAIL}" ]; then
        echo "GH_EMAIL not set"
        exit 1
    fi
    if [ -z "${GH_USER}" ]; then
        echo "GH_USER not set"
        exit 1
    fi
    if [ -z "${GH_PASSWORD}" ]; then
        echo "GH_USER not set"
        exit 1
    fi

    genernate_binding_codes

    cd $COCOS2DX_ROOT/tools/travis-scripts
    ./generate-cocosfiles.sh

    generate_pull_request_for_binding_codes
}

# build pull request
if [ "$TRAVIS_PULL_REQUEST" != "false" ]; then
    run_pull_request
fi

# run after merging
# - make cocos robot to send PR to cocos2d-x for new binding codes
# - generate cocos_files.json for template
if [ "$TRAVIS_PULL_REQUEST" == "false" ]; then
    run_after_merge
fi
