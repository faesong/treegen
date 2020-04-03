#!/bin/bash

DEPLOYED_PROJECT_NAME=treegen

# https://stackoverflow.com/questions/4774054/reliable-way-for-a-bash-script-to-get-the-full-path-to-itself
SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

def_path=~/wip/$DEPLOYED_PROJECT_NAME-build
def_urhobits_path=$(readlink -m "$SCRIPTPATH/../../urhobits")
def_cppbits_path=$(readlink -m "$SCRIPTPATH/../../cppbits")
def_rbfx_path=~/wip/rbfx-dist

deploy_path=""
urhobits_path=""
cppbits_path=""
rbfx_path=""

read -ep "Where to deploy the build ($def_path): " deploy_path
read -ep "Path to cppbits  ($def_cppbits_path): " cppbits_path
read -ep "Path to urhobits ($def_urhobits_path): " urhobits_path
read -ep "Path to rbfx ($def_rbfx_path): " rbfx_path

deploy_path="${deploy_path/#\~/$HOME}"
cppbits_path="${cppbits_path/#\~/$HOME}"
urhobits_path="${urhobits_path/#\~/$HOME}"
rbfx_path="${rbfx_path/#\~/$HOME}"

if [[ -z "$deploy_path" ]]
then
    deploy_path=$def_path
    echo $deploy_path
fi

if [[ -z "$cppbits_path" ]]
then
    cppbits_path=$def_cppbits_path
fi

if [[ -z "$urhobits_path" ]]
then
    urhobits_path=$def_urhobits_path
fi

if [[ -z "$rbfx_path" ]]
then
    rbfx_path=$def_rbfx_path
fi

assert_path () {
    if [ ! -d $1 ]
    then
        echo no such path: "$1"
        exit 1
    fi
}

assert_path $cppbits_path
assert_path $urhobits_path
assert_path $rbfx_path


echo "Deploy path: $deploy_path"
echo "cppbits path: $cppbits_path"
echo "urhobits path: $urhobits_path"
echo "rbfx path: $rbfx_path"

cd $(readlink -m "$deploy_path/../")
mkdir -p $(basename $deploy_path)
assert_path $(realpath "$deploy_path/../")/$(basename $deploy_path)

cd $(basename $deploy_path)

cmake -DRBFX_PATH=$rbfx_path -DURHOBITS_FOLDER=$urhobits_path -DCPPBITS_FOLDER=$cppbits_path $SCRIPTPATH/..


