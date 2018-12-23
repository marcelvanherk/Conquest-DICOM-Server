#!/bin/bash
#Script to push build results on the repository gh-pages branch.

# we should be in /home/travis/build/ivmartel/dwv-simplistic
echo -e "Starting to update gh-pages\n"

# go to home and setup git
cd $HOME
git config --global user.email "travis@travis-ci.org"
git config --global user.name "Travis"
# using token, clone gh-pages branch
git clone --quiet --branch=gh-pages https://${GH_TOKEN}@github.com/ivmartel/dwv-simplistic.git gh-pages
# change dir to demo
cd $HOME/gh-pages/demo/trunk
# clean up
rm -Rf *
# copy new build here
cp -Rf $HOME/build/ivmartel/dwv-simplistic/* .
# remove gitignore
rm -f .gitignore
# clean up node_modules
rm -Rf node_modules/*
# clean yarn to force update
rm -f yarn.lock
# download production dependencies
yarn install --prod
# move back to root of repo
cd $HOME/gh-pages
# add, commit and push files
git add -Af .
git commit -m "Travis build $TRAVIS_BUILD_NUMBER pushed to gh-pages"
git push -fq origin gh-pages

echo -e "Done updating.\n"
