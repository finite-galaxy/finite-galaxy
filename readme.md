# Finite Galaxy
**Finite Galaxy** is a derivative of [Endless Sky](https://github.com/endless-sky/endless-sky), a free and open source space exploration, trading, and combat game created by Michael Zahniser.
Both games can be installed alongside and played independently of each other.

## Table of contents
* https://github.com/finite-galaxy/finite-galaxy#contributing
* https://github.com/finite-galaxy/finite-galaxy#posting-issues
* https://github.com/finite-galaxy/finite-galaxy#posting-pull-requests
* https://github.com/finite-galaxy/finite-galaxy#changes
* https://github.com/finite-galaxy/finite-galaxy#major-changes
* https://github.com/finite-galaxy/finite-galaxy#minor-changes
* https://github.com/finite-galaxy/finite-galaxy#not-yet-implemented-ideas


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
* Add "Licences" tab in player info panel
* Add "Tribute" tab in player info panel
* Separate fleet overview column in outfitter and shipyard from ship info display
* Increase fleet overview column width from four to five ship icons
* Make hyperjump fuel proportional to ship's total mass
* Make ship explosion 'weapon' proportional to mass (base, empty, or total mass)

