# Finite Galaxy
**Finite Galaxy** is a derivative of [Endless Sky](https://github.com/endless-sky/endless-sky), a free and open source space exploration, trading, and combat game created by Michael Zahniser.
Both games can be installed alongside and played independently of each other.

## Table of contents
* https://github.com/finite-galaxy/finite-galaxy#installation
* https://github.com/finite-galaxy/finite-galaxy#gnu?linux
* https://github.com/finite-galaxy/finite-galaxy#microsoft?windows
* https://github.com/finite-galaxy/finite-galaxy#apple?mac-os-x
* https://github.com/finite-galaxy/finite-galaxy#gnu-linux
* https://github.com/finite-galaxy/finite-galaxy#contributing
* https://github.com/finite-galaxy/finite-galaxy#posting-issues
* https://github.com/finite-galaxy/finite-galaxy#posting-pull-requests
* https://github.com/finite-galaxy/finite-galaxy#changes
* https://github.com/finite-galaxy/finite-galaxy#major-changes
* https://github.com/finite-galaxy/finite-galaxy#minor-changes
* https://github.com/finite-galaxy/finite-galaxy#not-yet-implemented-ideas
* https://github.com/finite-galaxy/finite-galaxy#heliarch-invasion



## Installation

### GNU/Linux
Dependencies (version numbers may vary depending on your distribution):

DEB-based distros:
   g++ \
   scons \
   libsdl2-dev \
   libpng-dev \
   libjpeg-dev \
   libgl1-mesa-dev \
   libglew-dev \
   libopenal-dev \
   libmad0-dev

RPM-based distros:
   gcc-c++ \
   scons \
   SDL2-devel \
   libpng-devel \
   libjpeg-turbo-devel \
   mesa-libGL-devel \
   glew-devel \
   openal-soft-devel \
   libmad-devel

Open your terminal and enter:
* `git clone https://github.com/finite-galaxy/finite-galaxy.git` to get a local copy of the repository
* `cd finite-galaxy/` to open the directory
* `git pull` to update the game
* `scons` to compile the game
* `./finite-galaxy` to run the game

For more help, consult the man page (the finite-galaxy.6 file).

### Microsoft/Windows

The Windows build has been tested on 64-bit Windows 7, only. You will need the Code::Blocks IDE and g++ 4.8 or higher. Code::Blocks is available here:

  http://sourceforge.net/projects/codeblocks/files/Binaries/13.12/Windows/codeblocks-13.12-setup.exe/download

You can install g++ separately through mingw-w64:

  http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/4.8.5/threads-posix/seh/

*** Be sure to install the "pthread" version of MinGW. The "win32-thread" one does not come with support for C++11 threading. If you are using 32-bit Windows, install the compiler for "dwarf" exceptions, not "sjlj." ***

If you are on 64-bit Windows, a full set of development libraries are available here:

  http://endless-sky.github.io/win64-dev.zip

If you don't want to have to edit the paths in the Code::Blocks file, unpack the "dev64" folder directly into C:\.

If you are using 32-bit Windows, a full set of development libraries are available here:

  http://endless-sky.github.io/win32-dev.zip

You will probably need to adjust the paths to your compiler binaries, and you should also switch to the "Win32" build instead of the "Debug" or "Release" build.

You will also need libmingw32.a and libopengl32.a. Those should be included in the MinGW g++ install. If they are not in C:\Program Files\mingw64\x86_64-w64-mingw32\lib\ you will have to adjust the paths in the Code::Blocks file.

### Apple/Mac OS X

To build Finite Galaxy, you will first need to download Xcode from the App Store.

Next, install Homebrew (from http://brew.sh).

Once Homebrew is installed, use it to install the libraries you will need:
```
brew install libpng
brew install libjpeg-turbo
brew install libmad
brew install sdl2
```

If the versions of those libraries are different from the ones that the Xcode project is set up for, you will need to modify the file paths in the “Frameworks” section in Xcode.

It is possible that you will also need to modify the “Header Search Paths” and “Library Search Paths” in “Build Settings” to point to wherever Homebrew installed those libraries.

Library paths

To create a Mac OS X binary that will work on systems other than your own, you may also need to use `install_name_tool` to modify the libraries so that their location is relative to the @rpath.
```
sudo install_name_tool -id "@rpath/libpng16.16.dylib" /usr/local/lib/libpng16.16.dylib
sudo install_name_tool -id "@rpath/libmad.0.2.1.dylib" /usr/local/lib/libmad.0.2.1.dylib
sudo install_name_tool -id "@rpath/libturbojpeg.0.dylib" /usr/local/opt/libjpeg-turbo/lib/libturbojpeg.0.dylib
sudo install_name_tool -id "@rpath/libSDL2-2.0.0.dylib" /usr/local/lib/libSDL2-2.0.0.dylib
```



## Contributing
Contributions are welcome; anyone can contribute; feel free to open issues or make pull requests.

### Posting issues

The [issues page](https://github.com/finite-galaxy/finite-galaxy/issues) on GitHub is for tracking bugs and for art, content, and feature requests. When posting a new issue, please:

* Be polite and always assume good faith.
* Check to make sure it's not a duplicate of an existing issue.
* Create a separate "issue" for each bug and each request.

If requesting a new feature, first ask yourself: will this make the game more fun or interesting? Remember that this is a game, not a simulator. Changes will not be made purely for the sake of realism, especially if they introduce needless complexity or aggravation.

If you believe your issue has been resolved, you can close the issue yourself.

### Posting pull requests

If you are posting a pull request, please:

* Do not combine multiple unrelated changes into a single pull.
* Check the diff and make sure the pull request does not contain unintended changes.
* If changing the C++ code, follow the [coding standard](http://endless-sky.github.io/styleguide/styleguide.xml).
* Do not use tabs; use two spaces instead.
* Use Oxford spelling (the variant of English used by many scientific journals and international organizations such as the United Nations), instead of American, British, Canadian, or other national varieties.

If proposing a major pull request, start by posting an issue and discussing the best way to implement it. Often the first strategy that occurs to you will not be the cleanest or most effective way to implement a new feature.



## Changes
<!--
   ! " # $ % & ' ( )
 * + , − . / 0 1 2 3
 4 5 6 7 8 9 : ; < =
 > ? @ A B C D E F G
 H I J K L M N O P Q
 R S T U V W X Y Z [
 \ ] ^ _ ` a b c d e
 f g h i j k l m n o
 p q r s t u v w x y
 z { | } ~ ⌷ · ° ′ ″
 „ ‚ “ ‘ ” ’ « ‹ » ›
-->
<!--
   0,   28,   56,   84,  112,  140,  168,  196,  224,  252, 
 280,  308,  336,  364,  392,  420,  448,  476,  504,  532, 
 560,  588,  616,  644,  672,  700,  728,  756,  784,  812, 
 840,  868,  896,  924,  952,  980, 1008, 1036, 1064, 1092, 
1120, 1148, 1176, 1204, 1232, 1260, 1288, 1316, 1344, 1372, 
1400, 1428, 1456, 1484, 1512, 1540, 1568, 1596, 1624, 1652, 
1680, 1708, 1736, 1764, 1792, 1820, 1848, 1876, 1904, 1932, 
1960, 1988, 2016, 2044, 2072, 2100, 2128, 2156, 2184, 2212, 
2240, 2268, 2296, 2324, 2352, 2380, 2408, 2436, 2464, 2492, 
2520, 2548, 2576, 2604, 2632, 2660, 2688, 2716, 2744, 2772, 
2800, 2828, 2856, 2884, 2912, 2940, 2968, 2996, 3024, 3052
-->

### Major changes
* Installed outfits can no longer be plundered by default; outfits in cargo still can
* Minimum depreciation value raised to 50%, time lowered to one year
* Ship info display shows more stats
* Categories moved to data files; new ship categories based on total mass
* More regional specialization of outfitters and shipyards

### Minor changes
See `changelog.txt`, `ship_overview.txt`, and https://github.com/finite-galaxy/finite-galaxy/commits/master

### Not yet implemented ideas
* Add support for Unicode and different writing directions
* Add ligatures
* Add "Licences" tab in player info panel
* Add "Tribute" tab in player info panel
* Separate fleet overview column in outfitter and shipyard from ship info display
* Sort outfits and ships by size or mass instead of alphabetically
* Add "Manufacturer" to ships
* Add quotation mark preference
* Make hyperjump fuel proportional to ship's total mass
* Make ship explosion 'weapon' proportional to mass (base, empty, or total mass)
* Rename "New {Country}" planets to less culturally biased names

#### Heliarch Invasion
Another idea is to introduce a new Heliarch Invasion story-line. It starts when you land on Ring of Power after finishing the Free Worlds storyline and doing at least a hundred Coalition jobs. The Heliarch then ask you to bring them twelve jump drives and offer 50 million in return. Afterwards they enter human space and start constructing a Coalition base on Poisonwood (Graffias). Every day twelve ships jump in, land, and send three ships back with the other jump drives in cargo. The Heliarch fleet steadily grows and starts patrolling nearby systems. When you go back to Ring of Friendship to ask politely what's going on, they answer that Coalition space is ruled by the Heliarchs, therefore there is peace and order, whereas human space is controlled by the Quarg, therefore there is chaos and violence. You reply that most humans are peaceful, they answer some of their ships were attacked nonetheless, by "Quarg minions" (pirates, you guess). The Heliarchs have hope humans might become the fourth species to join the Coalition in the distant future, but right now they believe humanity is not yet ready. A few months later the Heliarchs "liberate" (conquer) the Greenrock (Shaula), "pacify" (destroy) the pirate facilities there, and "evacuates" (deports) its population to Solace (Pherkad). They start constructing a Heliarch naval base there and Coalition convoys between Graffias and Solace are now a regular phenomenon. You get increasingly worried. Still later the Heliarch attack and destroy the Quarg station in Tarazed, but are repulsed in Enif. You travel to Pug Iyik to ask the Pug for help. They answer the Pug are no friends of the Quarg and have no contact with the Heliarchs, therefore they are not inclined to intervene. You reply half-heartedly that you don't object to the Heliarch and Quarg fighting each other, but that humanity is caught in between. The Pug seem to chuckle but say nothing more. You leave, seemingly empty-handed, and decide to visit the Quarg ringworld on Hevru Hai, to explain the situation to them. The Quarg say they know what you've done; if anything, the Quarg seems amused rather than annoyed. You don't understand and go back to human space. Later you discover a wormhole has been opened, possibly by the Pug, connecting the Heliarch ringworlds with the Quarg systems near Tarazed. The Heliarchs perceive it as some Quarg trick and now faced with a Quarg invasion of Coalition space, they signal they are willing to negotiate with the Quarg, in secret. You travel to Lagrange to pick up a Quarg delegation in Lagrange (Enif), which turns out to be a single individual, and bring him secretly to the new Heliarch base on Greenrock (Shaula). There you are allowed to stay during the negotiations as a neutral witness. The Heliarchs say they hope the Quarg won't use the new wormhole and offer to do likewise; furthermore, they state that they intend to maintain their presence on Poisonwood (Graffias) to allow humans to get used to the Coalition; keep possession of Shaula to be able to protect humanity from there; but promise not to move beyond Lesath, Wei, and Zubenelgenubi; after all, the Heliarch only intend to help humanity, not to conquer new territory for themselves. The Quarg does little more than merely nodding. It seems they've somehow reached an agreement, although the talks seem rather alien to ordinary humans. You bring back the Quarg to Lagrange and the Heliarchs inform you they'll reward you by allowing you to purchase some Heliarch technology on Poisonwood, but not weapons or warships. Meanwhile human governments fail to understand what's going on but are afraid both of the Heliarchs and of the Quarg. As a consequence of the brief conflict the Free Worlds are practically partitioned into:
* A Bourne-based neutral core in the northern Rim; Free Worlds Dreadnoughts permanently present in Zubeneschamali. 
* A Solace-based Heliarch protectorate in the south-west, where Heliarch fleets patrol systems and eliminated piracy; Heliarch Punishers permanently present in Shaula, Lesath, Sargas, Kornephoros, and Sabik.
* A Wayfarer-based pro-Quarg faction in south-east; Quarg Wardragons permanently present in Nunki, Girtab, Rastaban, and Delta Sagittarii.
* Some other Dirt Belt systems feel left out and decide to rejoin the Republic; Navy Jupiters permanently present in Alioth, Seginus, and Kaus Borealis.
Furthermore, there is still that wormhole with a large Quarg fleet on one end and a large Heliarch fleet on the other, but no-one travelling through it. Although there is no official peace treaty, the ceasefire appears to hold.

