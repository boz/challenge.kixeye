# kixeye 24 hour coding challenge: Write a socket server.

I wrote this from scratch in a night, using only ANSI-C and
POSIX standard libraries.

## the app: talk client & server

It's an extremely minimal talk/irc client.  All messages are broadcast
to all users.

### example session

#### console 1

    $ ./server

#### console 2

    $ ./client -n luser1
    >>> --=[ luser2 signed on ]=--
    sup y'all
    >>> --=[ luser2 signed off ]=--

#### console 3

    $ ./client -n luser2
    >>> luser1: sup y'all
    ^C

## What I would do if I had more time

 * Better message format.  Messages with payloads that are more
   complicated than a string are difficult to create.
 * Async send().  Right now it's doing a blocking send and assuming that
   all data gets written.  Fixing this would mean storing more state
   about the outgoing message, which is done in a completely ad-hock and
   hacky way for the reads.
 * Better client-side UI.  I'd like to have a std scrolling chat log and
   fixed-position input.  Also, colors.
 * signal handling and graceful shutdown.
 * better error handling in general.

## What features I would add if I had weeks

 * I'd start over and write it with proven event, console, and data
   structure libraries.
 * more commands: direct message, moderation.
 * plug-in architecture for adding commands.

