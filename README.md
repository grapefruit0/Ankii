# Ankii

Ankii can make anki English cards in batches according to [cambridge dictionary](https://dictionary.cambridge.org/)


## Dependency

On Debian/Ubuntu, install the following packages

        - wget
        - pandoc

## Building

To build Ankii, simply run the 'make'

        $ make

## Usage

Tips: the computer network connection should be normal when using Ankii

First, put the word you want to make the card with write file anki-list by line, such as

        $ cat anki-list
        simple
        hard

To use Ankii, simply run the command: 

        $ ./ankii anki-list

Finally, import file Anki\_20220416\_003130.csv into Anki
