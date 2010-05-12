#! /bin/bash


VERSION="0.08_beta_mac"
DISTRI_DIR="_distri"


function BuildAndPack {
  xcodebuild clean build -configuration "$2" -sdk "$1"
  if [ "$?" -en 0 ]; then exit 1; fi 
  cp "$2/LuxC4D.dylib" "$DISTRI_DIR/LuxC4D"
  CUR_DIR=`pwd`
  cd "$DISTRI_DIR"
  zip -9 -r "LuxC4D_${VERSION}_${3}.zip" LuxC4D
  cd "$CUR_DIR"
}


rm -fR "$DISTRI_DIR"
mkdir -p "$DISTRI_DIR/LuxC4D"
cp -R ../../res "$DISTRI_DIR/LuxC4D"
cp -R ../../license "$DISTRI_DIR/LuxC4D"


# BuildAndPack "macosx10.4" "R9.6_Debug"    "r9.6_debug"
BuildAndPack "macosx10.4" "R9.6_Release"  "r9.6_release"
# BuildAndPack "macosx10.4" "R10.1_Debug"   "r10.1_debug"
BuildAndPack "macosx10.4" "R10.1_Release" "r10.1_release"
# BuildAndPack "macosx10.5" "R10.5_Debug"   "r10.5_debug"
BuildAndPack "macosx10.5" "R10.5_Release" "r10.5_release"
# BuildAndPack "macosx10.5" "R11.0_Debug"   "r11.0_debug"
BuildAndPack "macosx10.5" "R11.0_Release" "r11.0_release"
# BuildAndPack "macosx10.5" "R11.5_Debug"   "r11.5_debug"
BuildAndPack "macosx10.5" "R11.5_Release" "r11.5_release"


rm -fR "$DISTRI_DIR/LuxC4D"
