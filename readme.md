# Finite Galaxy
**Finite Galaxy** is a free and open source space exploration game; the repository containing all files is located at https://github.com/finite-galaxy/finite-galaxy/
It is derived from *Endless Sky*, a game created by Michael Zahniser, which is located at https://github.com/endless-sky/endless-sky/

Both games can be installed alongside and played independently of each other. Although derived from the same source code and basically using the same content, Finite Galaxy and Endless Sky have diverged and are no longer compatible. If you transpose a save game from one to the other, you are likely to encounter hundreds of errors.



## Table of contents
* [Introduction](https://github.com/finite-galaxy/finite-galaxy#introduction)
* [Installation](https://github.com/finite-galaxy/finite-galaxy#installation)
  * [GNU/Linux](https://github.com/finite-galaxy/finite-galaxy#gnu-linux)
  * [Apple/Mac OS X](https://github.com/finite-galaxy/finite-galaxy#apple-mac-os-x)
  * [Microsoft Windows](https://github.com/finite-galaxy/finite-galaxy#microsoft-windows)
* [Changes](https://github.com/finite-galaxy/finite-galaxy#changes)
  * [Major changes](https://github.com/finite-galaxy/finite-galaxy#major-changes)
  * [Minor changes](https://github.com/finite-galaxy/finite-galaxy#minor-changes)
  * [Not yet implemented ideas](https://github.com/finite-galaxy/finite-galaxy#not-yet-implemented-ideas)
* [Contributing](https://github.com/finite-galaxy/finite-galaxy#contributing)
  * [Posting issues](https://github.com/finite-galaxy/finite-galaxy#posting-issues)
  * [Posting pull requests](https://github.com/finite-galaxy/finite-galaxy#posting-pull-requests)
* [Reveal entire map](https://github.com/finite-galaxy/finite-galaxy#reveal-entire-map)



## Introduction
Why did I start this project? Why not contribute to Endless Sky instead?
* Although I like Endless Sky as a whole, it also contains things I don't like. Conversation scenes, news and portraits, tribute, and plundering of installed outfits are just a few examples.
* The pace of development seemed to slow down: there were only nine commits in September 2018 and zero in October. (I started Finite Galaxy on October 18.)
* Numerous pull requests over there have been open for over a year, reviews are haphazard, there are many lengthy discussions on unimportant things, while useful proposals were often ignored.
* The direction and vision is not always clear.
* Support for plug-ins is rather limited.

In short, I consider it a better use of time to work on a project where I can incorporate most of my ideas, where I can remove things I dislike, and where I can contribute whenever I like, without having to wait weeks or months for a review or wasting my time on something that won't be included. 

Finite Galaxy is very much a work in progress. Nevertheless, it can be compiled and played without errors. Feel free to try it out yourself!

[(return to top)](https://github.com/finite-galaxy/finite-galaxy#finite-galaxy)



## Installation

### GNU Linux
Dependencies (version numbers may vary depending on your distribution):

DEB-based distros: \
   g++ \
   scons \
   libsdl2-dev \
   libpng-dev \
   libjpeg-dev \
   libgl1-mesa-dev \
   libglew-dev \
   libopenal-dev \
   libmad0-dev

RPM-based distros: \
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

For more help, consult the man page (the `finite-galaxy.6` file).

[(return to top)](https://github.com/finite-galaxy/finite-galaxy#finite-galaxy)


### Apple Mac OS X
*If you have trouble compiling or encounter errors, please post* [here](https://github.com/finite-galaxy/finite-galaxy/issues/18).

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

[(return to top)](https://github.com/finite-galaxy/finite-galaxy#finite-galaxy)


### Microsoft Windows
*If you have trouble compiling or encounter errors, please post* [here](https://github.com/finite-galaxy/finite-galaxy/issues/17).

You will need the Code::Blocks IDE and g++ 4.8 or higher. Code::Blocks is available here:

  http://sourceforge.net/projects/codeblocks/files/Binaries/13.12/Windows/codeblocks-13.12-setup.exe/download

You can install g++ separately through mingw-w64:

  http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/4.8.5/threads-posix/seh/

*** Be sure to install the "pthread" version of MinGW. The "win32-thread" one does not come with support for C++14 threading. If you are using 32-bit Windows, install the compiler for "dwarf" exceptions, not "sjlj." ***

If you are on 64-bit Windows, a full set of development libraries are available here:

  http://endless-sky.github.io/win64-dev.zip

If you don't want to have to edit the paths in the Code::Blocks file, unpack the "dev64" folder directly into C:\.

If you are using 32-bit Windows, a full set of development libraries are available here:

  http://endless-sky.github.io/win32-dev.zip

You will probably need to adjust the paths to your compiler binaries, and you should also switch to the "Win32" build instead of the "Debug" or "Release" build.

You will also need libmingw32.a and libopengl32.a. Those should be included in the MinGW g++ install. If they are not in C:\Program Files\mingw64\x86_64-w64-mingw32\lib\ you will have to adjust the paths in the Code::Blocks file.

[(return to top)](https://github.com/finite-galaxy/finite-galaxy#finite-galaxy)



## Changes

### Major changes
* Hyperjump fuel is based on your ship's effective mass (including cargo and carried ships)
* Hyperjump fuel is no longer free, its price depends on the planet (when landing) or the government (when hailing ships in space)
* Ships continuously consume energy, based upon the number of bunks, to represent life-support
* Ship categories are based on total mass
  * `ship mass = hull mass + outfit space + cargo space`
* Introduced core space, reserved for energy generators, shields and hull systems, and hyperdrives
  * `outfit space = core space + engine space + weapon space`
* Installed outfits can no longer be plundered by default; outfits in cargo still can
* Minimum depreciation value raised to 50%, time lowered to one year
* Weapon projectile damage is a random number between `damage` and `damage + random damage`
* Ship info display shows more stats
* Redistributed most human ships and many outfits to have more regional specialization
* Removed tribute from planets (relevant code is still present for plug-ins)
* Removed news and portraits (relevant code is still present for plug-ins)
* Distances from planets to the sytem's centre are trebled; as a result space feels larger, thrusters are more desirable, and players won't always land immediately in the middle of a space fight
* Non-missile weapons have their weapon range increased by about a third
* Exploding ships are significantly more dangerous
* Add support for Unicode and different writing directions

[(return to top)](https://github.com/finite-galaxy/finite-galaxy#finite-galaxy)


### Minor changes
See `changelog.txt`, `ship_overview.txt`, and https://github.com/finite-galaxy/finite-galaxy/commits/master

[(return to top)](https://github.com/finite-galaxy/finite-galaxy#finite-galaxy)


### Not yet implemented ideas
* Add quotation mark preference
* Add “Licences” tab in player info panel
* Add “Tribute” tab in player info panel
* Add “Manufacturer” to ships
* Add functionality to deposit credits at the bank for a fixed time (e.g. one year), receiving either the sum plus interest when it expires, or the sum minus a penalty when you claim it beforehand
* Add planet landing fees support
* Allow friendly fire
* Allow sorting available jobs (by e.g. cargo size, distance, name, payment, etc.)
* Allow sorting outfits by cost, mass, name
* Allow sorting ships by cost, mass, name, outfit space, shields, etc.
* Display flagship speed by default and display target's speed with tactical scanner
* De-hardcode jump radius
* Increase jump radius if you have multiple jump drives installed, perhaps 100*(jump drive)^0.5
* Limit the commodities for sale on specific planets
* Make ship explosion ‘weapon’ automatically proportional to mass (base, empty, or total mass)
* Separate fleet overview column in outfitter and shipyard from ship info display
* Separate slots for guns and missile launchers
* Ships entering a system from hyperspace should be positioned near the system's centre, instead of near the first inhabited planet
* https://github.com/endless-sky/endless-sky/wiki/DevelopmentRoadmap

[(return to top)](https://github.com/finite-galaxy/finite-galaxy#finite-galaxy)

<!--
#### Syndicate branch
Currently there are two possible endings to the Free Worlds storyline. In the Checkmate branch, you ignore the Syndicate defector, focus on fighting the Republic, unlock nuclear missiles, and prepare an assault on Earth, upon which the Pug intervene. In the Reconciliation branch, you pick up the Syndicate defector, bring him to safety in the Deep, lead a combined Deep Security-Free Worlds-Republic Navy police action against the Syndicate, where you are gifted a Jump Drive and a Cloaking device, to solve the Pug invasion. But what if there was a third option? 
You could speak with the Syndicate defector, learn there is a one million credits bounty on his head, return him to the Syndicate, get the reward, and disappoint the Free Worlds Council. Afterwards the Syndicate recruits you by offering you more than twice the salary, you accept, change colours, assist the Syndicate in conquering a pirate world. Because the Navy suffered several defeats against the Free Worlds and a large part of their personnel joined the Oathkeepers, the Republic hires the Syndicate to fight the Free Worlds, offering them exclusive trade rights in certain systems. You join the Syndicate invasion fleet, but then plans suddenly change because of the appearance of the Pug.

[(return to top)](https://github.com/finite-galaxy/finite-galaxy#finite-galaxy)
-->

<!--
#### Heliarch Invasion
Another idea is to introduce a new Heliarch Invasion story-line. It starts when you land on Ring of Power after finishing the Free Worlds storyline and doing at least a hundred Coalition jobs. The Heliarch then ask you to bring them twelve jump drives and offer 50 million in return. Afterwards they enter human space and start constructing a Coalition base on Poisonwood (Graffias). Every day twelve ships jump in, land, and send three ships back with the other jump drives in cargo. The Heliarch fleet steadily grows and starts patrolling nearby systems. When you go back to Ring of Friendship to ask politely what's going on, they answer that Coalition space is ruled by the Heliarch, therefore there is peace and order, whereas human space is controlled by the Quarg, therefore there is chaos and violence. You reply that most humans are peaceful, they answer some of their ships were attacked nonetheless, by “Quarg minions” (pirates, you guess). The Heliarch have hope humans might become the fourth species to join the Coalition in the distant future, but right now they believe humanity is not yet ready. A few months later the Heliarch “liberate” (conquer) Greenrock (Shaula), “pacify” (destroy) the pirate facilities there, and “evacuate” (deports) its population to Solace (Pherkad). They start constructing a Heliarch naval base there and Coalition convoys between Graffias and Solace are now a regular phenomenon. You get increasingly worried. Still later the Heliarch attack and destroy the Quarg station in Tarazed, but are repulsed in Sadachbia. You travel to Pug Iyik to ask the Pug for help. They answer the Pug are no friends of the Quarg and have no contact with the Heliarch, therefore they are not inclined to intervene. You reply half-heartedly that you don't object to the Heliarch and Quarg fighting each other, but that humanity is caught in between. The Pug seem to chuckle but say nothing more. You leave, seemingly empty-handed, and decide to visit the Quarg ringworld on Hevru Hai, to explain the situation to them. The Quarg say they know what you've done; if anything, the Quarg seems amused rather than annoyed. You don't understand and go back to human space. Later you discover a wormhole has been opened, possibly by the Pug, connecting Ruwarku with Sadachbia. The Heliarch perceive it as some Quarg trick and now faced with a Quarg invasion of Coalition space, they signal they are willing to negotiate with the Quarg, in secret. You travel to Lagrange to pick up a Quarg delegation in Lagrange (Sadachbia), which turns out to be a single individual, and bring him secretly to the new Heliarch base on Greenrock (Shaula). There you are allowed to stay during the negotiations as a neutral witness. The Heliarch say they hope the Quarg won't use the new wormhole and offer to do likewise; furthermore, they state that they intend to maintain their presence on Poisonwood (Graffias) to allow humans to get used to the Coalition; keep possession of Shaula to be able to protect humanity from there; but promise not to move beyond Lesath, Wei, and Zubenelgenubi; after all, the Heliarch only intend to help humanity, not to conquer new territory for themselves. The Quarg does little more than merely nodding. It seems they've somehow reached an agreement, although the talks seem rather alien to ordinary humans. You bring back the Quarg to Lagrange and the Heliarch inform you they'll reward you by allowing you to purchase some Heliarch technology on Poisonwood, but not weapons or warships. Meanwhile human governments fail to understand what's going on but are afraid both of the Heliarch and of the Quarg. As a consequence of the brief conflict the Free Worlds are practically partitioned into:
* A Bourne-based neutral core in the northern Rim; Free Worlds Dreadnoughts permanently present in Zubeneschamali. 
* A Solace-based Heliarch protectorate in the south-west, where Heliarch fleets patrol systems and eliminated piracy; Heliarch Punishers permanently present in Shaula, Lesath, Sargas, Kornephoros, and Sabik.
* A Wayfarer-based pro-Quarg faction in south-east; Quarg Wardragons permanently present in Nunki, Girtab, Rastaban, and Delta Sagittarii.
* Some other Dirt Belt systems feel left out and decide to rejoin the Republic; Navy Jupiters permanently present in Alioth, Seginus, and Kaus Borealis.
Furthermore, there is still that wormhole with a large Quarg fleet on one end and a large Heliarch fleet on the other, but no-one travelling through it. Although there is no official peace treaty, the ceasefire appears to hold.

[(return to top)](https://github.com/finite-galaxy/finite-galaxy#finite-galaxy)
-->



## Contributing
Contributions are welcome; anyone can contribute; feel free to open issues or make pull requests.

### Posting issues
The [issues page](https://github.com/finite-galaxy/finite-galaxy/issues) on GitHub is for tracking bugs and for art, content, and feature requests. When posting a new issue, please:
* Be polite and always assume good faith.
* Check to make sure it's not a duplicate of an existing issue.
* Create a separate “issue” for each bug, problem, question, or request.

If requesting a new feature, first ask yourself: will this make the game more fun or interesting? Remember that this is a game, not a simulator. Changes will not be made purely for the sake of realism, especially if they introduce needless complexity or aggravation.

If you believe your issue has been resolved, you can close the issue yourself.

[(return to top)](https://github.com/finite-galaxy/finite-galaxy#finite-galaxy)


### Posting pull requests
If you are posting a pull request, please:
* Do not combine multiple unrelated changes into a single pull.
* Check the diff and make sure the pull request does not contain unintended changes.
* If proposing a major pull request, start by posting an issue and discussing the best way to implement it. Often the first strategy that occurs to you will not be the cleanest or most effective way to implement a new feature.
* `code/`:
  * follow the [coding standard](http://endless-sky.github.io/styleguide/styleguide.xml).
  * C++14
  * do not use tabs; use two spaces instead
  * make numbers with many digits easier to read for humans by inserting `'`
    * (decimal numbers) at intervals of three digits if there are more than four in a row
    * (hexadecimal numbers) at intervals of four digits if there are more than six in a row
  * use Oxford English
* `data/`:
  * do not use tabs; use two spaces instead
  * use Oxford spelling (the variant of English used by many scientific journals and international organizations such as the United Nations), instead of American, British, Canadian, or other national varieties.
  * no diacritics in English:
    * á, à, â → a; same for other vowels
    * å → aa
    * æ, ä → ae
    * œ, ø, ö → oe
    * ü → ue
    * İ/ı → I/i
    * ç → c
    * č, ć, ċ → ch
    * š, ś, ş → sh
    * ñ → nh
    * ß → ss
  * text strings (conversations, descriptions, mission dialogues, tooltips, etc.):
    * avoid abbreviations (e.g., i.e., etc.); contractions (isn't) are fine
    * use an Oxford comma when giving more than two items (e.g. one, two, and three; *not* one, two and three)
    * use U+2013 – en-dash for number ranges (e.g. 10–12) and for parenthetical expressions – like this – instead of parentheses, em-dashes, or hyphens
    * use U+2026 … horizontal ellipsis instead of three full stops
    * use U+202F   narrow non-breaking space as a thousands separator for numbers with five or more digits (e.g. 12 345)
    * use U+2212 − minus sign for negative numbers, subtractions, and deductions
    <!-- 0°1′2″3‴4⁗ -->
    * use the " quote for direct speech and ' apostrophe within direct speech; the source code replaces these with proper “primary” and ‘secondary’ opening and closing quotation marks; surround such strings with ` backticks
  * repeatedly check and double check any new or changed strings to avoid unnecessary typos; e.g. mind the difference between it's (cf. he's, she's) and its (cf. his, her).
* `images/`:
  * file names are lower case and use underscores instead of spaces
  * add both normal and @2x versions
    * for ships, also create [thumbnails](https://github.com/endless-sky/endless-sky/wiki/CreatingShips#shipyard-thumbnails)
    * for turrets, also create hardpoints
  * insert yourself in the copyright.txt file
  * include all assets (Blender, GIMP, other files) in the opening post
* `sounds/`:
  * file names are lower case and use underscores instead of spaces
  * insert yourself in the `copyright.txt` file

[(return to top)](https://github.com/finite-galaxy/finite-galaxy#finite-galaxy)



## Reveal entire map
Part of the fun of the game is travelling around and exploring. However, if you don't have time for that and simply want to reveal everything in the entire galaxy, then open your save game, find `# What you know:` and insert the following lines directly afterwards:

```
visited "1 Axis"
visited "10 Pole"
visited "11 Autumn Above"
visited "11 Spring Below"
visited "12 Autumn Above"
visited "14 Pole"
visited "14 Summer Above"
visited "14 Winter Below"
visited "16 Autumn Rising"
visited "3 Axis"
visited "3 Pole"
visited "3 Spring Rising"
visited "4 Axis"
visited "4 Spring Rising"
visited "4 Summer Rising"
visited "4 Winter Rising"
visited "5 Axis"
visited "5 Spring Below"
visited "5 Summer Above"
visited "5 Winter Above"
visited "7 Autumn Rising"
visited "8 Winter Below"
visited "9 Spring Above"
visited "Ablodab"
visited "Ablub"
visited "Acamar"
visited "Achernar"
visited "Acrux"
visited "Adhara"
visited "Aescolanus"
visited "Al Dhanab"
visited "Albaldah"
visited "Albireo"
visited "Alcyone"
visited "Aldebaran"
visited "Alderamin"
visited "Aldhibain"
visited "Algedi"
visited "Algenib"
visited "Algenubi"
visited "Algieba"
visited "Algol"
visited "Algorel"
visited "Alheka"
visited "Alhena"
visited "Alioth"
visited "Alkaid"
visited "Almaaz"
visited "Almach"
visited "Alnair"
visited "Alnasl"
visited "Alnilam"
visited "Alnitak"
visited "Alniyat"
visited "Alpha Arae"
visited "Alpha Centauri"
visited "Alpha Hydri"
visited "Alphard"
visited "Alphecca"
visited "Alpheratz"
visited "Altair"
visited "Aludra"
visited "Ancient Hope"
visited "Ankaa"
visited "Answer"
visited "Antares"
visited "Antevorta"
visited "Ap'arak"
visited "Arcturus"
visited "Arculus"
visited "Arneb"
visited "Ascella"
visited "Asikafarnut"
visited "Aspidiske"
visited "Atria"
visited "Avior"
visited "Aya'k'k"
visited "Beginning"
visited "Bellatrix"
visited "Belonging"
visited "Belug"
visited "Belugt"
visited "Beta Lupi"
visited "Betelgeuse"
visited "Bloptab"
visited "Blubipad"
visited "Blugtad"
visited "Boral"
visited "Bore Fah"
visited "Bote Asu"
visited "Bright Void"
visited "Broken Bowl"
visited "Caeculus"
visited "Canopus"
visited "Capella"
visited "Caph"
visited "Cardax"
visited "Cardea"
visited "Castor"
visited "Cebalrai"
visited "Celeborim"
visited "Chalawan"
visited "Charm"
visited "Chikatip"
visited "Chimitarp"
visited "Chirr'ay'akai"
visited "Chornifath"
visited "Chy'chra"
visited "Cinxia"
visited "Coluber"
visited "Companion"
visited "Convector"
visited "Cor Caroli"
visited "Da Ent"
visited "Da Lest"
visited "Dabih"
visited "Danoa"
visited "Dark Hills"
visited "Debrugt"
visited "Delta Capricorni"
visited "Delta Sagittarii"
visited "Delta Velorum"
visited "Deneb"
visited "Denebola"
visited "Diphda"
visited "Dokdobaru"
visited "Dschubba"
visited "Dubhe"
visited "Due Yoot"
visited "Durax"
visited "Eber"
visited "Eblumab"
visited "Edusa"
visited "Ehma Ti"
visited "Ek'kek'ru"
visited "Ekuarik"
visited "Elnath"
visited "Eltanin"
visited "Eneremprukt"
visited "Enif"
visited "Es'sprak'ai"
visited "Eshkoshtar"
visited "Eteron"
visited "Fah Root"
visited "Fah Soom"
visited "Fala"
visited "Fallen Leaf"
visited "Far Horizon"
visited "Farbutero"
visited "Farinus"
visited "Faronektu"
visited "Fasitopfar"
visited "Fell Omen"
visited "Feroteri"
visited "Ferukistek"
visited "Fingol"
visited "Flugbu"
visited "Fomalhaut"
visited "Fornarep"
visited "Four Pillars"
visited "Furmeliki"
visited "Gacrux"
visited "Gamma Cassiopeiae"
visited "Gamma Corvi"
visited "Gienah"
visited "Girtab"
visited "Glubatub"
visited "Gomeisa"
visited "Good Omen"
visited "Gorvi"
visited "Graffias"
visited "Gupta"
visited "Hadar"
visited "Hamal"
visited "Han"
visited "Hassaleh"
visited "Hatysa"
visited "Heia Due"
visited "Hesselpost"
visited "Hevru Hai"
visited "Hi Yahr"
visited "Hintar"
visited "Holeb"
visited "Homeward"
visited "Host"
visited "Hunter"
visited "Ik'kara'ka"
visited "Ildaria"
visited "Imo Dep"
visited "Insitor"
visited "Io Lowe"
visited "Io Mann"
visited "Ipsing"
visited "Iyech'yek"
visited "Izar"
visited "Ka'ch'chrai"
visited "Ka'pru"
visited "Kaliptari"
visited "Kappa Centauri"
visited "Kashikt"
visited "Kasikfar"
visited "Kaus Australis"
visited "Kaus Borealis"
visited "Ki War Ek"
visited "Kiro'ku"
visited "Kiru'kichi"
visited "Kochab"
visited "Kor Ak'Mari"
visited "Kor En'lakfar"
visited "Kor Fel'tar"
visited "Kor Men"
visited "Kor Nor'peli"
visited "Kor Tar'bei"
visited "Kor Zena'i"
visited "Kornephoros"
visited "Korsmanath"
visited "Kraz"
visited "Kugel"
visited "Kursa"
visited "Last Word"
visited "Lesath"
visited "Levana"
visited "Limen"
visited "Lolami"
visited "Lom Tahr"
visited "Lone Cloud"
visited "Lucina"
visited "Lurata"
visited "Makferuti"
visited "Markab"
visited "Markeb"
visited "Matar"
visited "Mebla"
visited "Mebsuta"
visited "Meftarkata"
visited "Mei Yohn"
visited "Mekislepti"
visited "Membulem"
visited "Men"
visited "Menkalinan"
visited "Menkar"
visited "Menkent"
visited "Merak"
visited "Mesuket"
visited "Miaplacidus"
visited "Miblulub"
visited "Mimosa"
visited "Minkar"
visited "Mintaka"
visited "Mirach"
visited "Mirfak"
visited "Mirzam"
visited "Mizar"
visited "Moktar"
visited "Mora"
visited "Muhlifain"
visited "Muphrid"
visited "Naos"
visited "Naper"
visited "Nashira"
visited "Nenia"
visited "Nihal"
visited "Nocte"
visited "Nunki"
visited "Oblate"
visited "Orbona"
visited "Orvala"
visited "Ossipago"
visited "Over the Rainbow"
visited "Pantica"
visited "Parca"
visited "Peacock"
visited "Pelubta"
visited "Peragenor"
visited "Peresedersi"
visited "Perfica"
visited "Persian"
visited "Persitar"
visited "Phact"
visited "Phecda"
visited "Pherkad"
visited "Phurad"
visited "Pik'ro'iyak"
visited "Plort"
visited "Polaris"
visited "Pollux"
visited "Porrima"
visited "Prakacha'a"
visited "Procyon"
visited "Pug Iyik"
visited "Quaru"
visited "Rajak"
visited "Rasalhague"
visited "Rastaban"
visited "Rati Cal"
visited "Regor"
visited "Regulus"
visited "Remembrance"
visited "Rigel"
visited "Ruchbah"
visited "Rutilicus"
visited "Ruwarku"
visited "Sabik"
visited "Sabriset"
visited "Sadachbia"
visited "Sadalmelik"
visited "Sadalsuud"
visited "Sadr"
visited "Sagittarius A*"
visited "Saiph"
visited "Salipastart"
visited "Salm"
visited "Sargas"
visited "Sarin"
visited "Sayaiban"
visited "Scheat"
visited "Schedar"
visited "Segesta"
visited "Seginus"
visited "Seketra"
visited "Sepetrosk"
visited "Sepriaptu"
visited "Sevrelect"
visited "Shaula"
visited "Sheratan"
visited "Si'yak'ku"
visited "Sich'ka'ara"
visited "Silikatakfar"
visited "Silver Bell"
visited "Silver String"
visited "Similisti"
visited "Sirius"
visited "Situla"
visited "Skeruto"
visited "Sko'karak"
visited "Sobarati"
visited "Sol"
visited "Sol Arachi"
visited "Sol Kimek"
visited "Sol Saryds"
visited "Solifar"
visited "Sospi"
visited "Speloog"
visited "Spica"
visited "Steep Roof"
visited "Stercutus"
visited "Suhail"
visited "Sumar"
visited "Sumprast"
visited "Tais"
visited "Talita"
visited "Tania Australis"
visited "Tarazed"
visited "Tarf"
visited "Tebuteb"
visited "Tejat"
visited "Terminus"
visited "Terra Incognita"
visited "Torbab"
visited "Tortor"
visited "Turais"
visited "Ula Mon"
visited "Ultima Thule"
visited "Umbral"
visited "Unagi"
visited "Unukalhai"
visited "Uwa Fahn"
visited "Vega"
visited "Vindemiatrix"
visited "Volax"
visited "Wah Ki"
visited "Wah Oh"
visited "Wah Yoot"
visited "Waypoint"
visited "Wazn"
visited "Wei"
visited "Wezen"
visited "World's End"
visited "Ya Hai"
visited "Yed Prior"
visited "Zaurak"
visited "Zeta Aquilae"
visited "Zeta Centauri"
visited "Zosma"
visited "Zuba Zub"
visited "Zubenelgenubi"
visited "Zubenelhakrabi"
visited "Zubeneschamali"
"visited planet" "Ember Reaches"
"visited planet" "Ember Threshold"
"visited planet" "Ember Wormhole"
"visited planet" "Hai Wormhole"
"visited planet" "Pirate Wormhole"
"visited planet" "Pug Wormhole"
"visited planet" "Quarg Wormhole"
"visited planet" "Remnant Wormhole"
"visited planet" "Rim Wormhole"
"visited planet" "The Eye"
```

[(return to top)](https://github.com/finite-galaxy/finite-galaxy#finite-galaxy)

