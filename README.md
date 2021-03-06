# Treegen
A tree generator.

_Last reviewed 18.04.2020 ( commit e85507c )_

## What is it already good for
Frankly, not much really. There are certain (simple) types of trees that can be
made and look quite realistic.

You can use this program to make yourself some trees for 3D renders (too many
polygons for games as of now), or you can incorporate it into your own
game/program for tree generation -- provided that it is fine for you that making
somewhat-realistic trees with this requires atleast 30k polygons per tree.

(The "Export" button should export to GLTF format if not specified otherwise)


![Blender screenshot](https://gitlab.com/fiasco/treegen/-/wikis/uploads/b6dfd37762150525b09a1425f5e7b125/Treegen1.png "Treegen in action")


![Blender screenshot](https://gitlab.com/fiasco/treegen/-/wikis/uploads/bee38c7bc63be3b85960d31e5fd3c543/TreesInBlender.png "Some trees exported (in gltf format) and imported into blender, applied custom Eevee material")

## Dependencies
* rbfx https://github.com/rokups/rbfx -- fork of Urho3D game engine
* VcppBits https://gitlab.com/fiasco/vcppbits -- my basic C++ stuff
* RbfxBits https://gitlab.com/fiasco/rbfxbits -- my Urho3D classes...

## Why write this?
The goal is to be able to generate trees for games at-the-go with as little
manual effort as possible.

## Plans

I try to order the list by the severety level, yet it's not entirely strict.

* Texturing of the trunk and branches is quite simplistic and practically 
  isn't really useful at all
* There are certain features that make many tree types impossible to make
* There is no automation for Level-Of-Detail (lower-poly version), and neither
  any useful metadata (like vertex groups or tree skeleton, ..) exported to aid
  in making manual lower-polycound LODs
* Using this as a library ( https://gitlab.com/fiasco/rbfxbits ) is only viable
  from rbfx game engine really. It is not really hard to generalize the code to
  use any kind of 3D engine.
* UI is just enough to be usable, but not greatly convenient or user friendly
  much.
* Performance can be improved a lot:
  * Many things can be threaded
  * Many things can be cached
  * Tree is regenerated on each edit, while it's possible to avoid that in many
    cases
* Naming and documentation is absent
* Not all strings are translatable yet
