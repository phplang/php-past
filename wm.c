/*
 * PHP Tools
 *
 * Personal Home Page Tools.
 *
 * Copyright (C) 1995, Rasmus Lerdorf <rasmus@io.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * If you do use this software, please let me know.  I'd like to maintain
 * a list of URL's on my home page just to see what interesting things
 * people are doing with their home pages these days.
 */
/*
 * wild_match has been borrowed from eggdrop 0.9m
 * by Robey Pointer <robey@annwfn.indstate.edu>
 *
 * I've just fixed up the prototype and whitespace to be
 * more readable. -Ben Eng <ben@achilles.net>
 */
/*
 * Yanked out C++ comments -Rasmus Lerdorf <rasmus@io.org>
 */

/* brand new reg.c -- this one seems to get a fairly consistant 10%
   gain over the old one.  i ripped off the one from IRC servers and
   edited it to make it work a little better (allows better quoting,
   and returns a number indicating how closely the string matched,
   just like the old one).   */
 
#include <stdio.h>
#include <sys/types.h>

#define tolower(c) \
	( ( ( ( c ) >= 'A' ) && ( ( c ) <= 'Z' ) ) ? ( ( c ) - 'A' + 'a' ) : c )

/* this is the matches( ) function from ircd, spaghetti'd up to be more
   effecient on masks that don't end with a wildcard ( ie: hostmask matches ).
   first it does a quick scan of the string in reverse, aborting quickly
   if any non-wildcard characters don't match.  once it comes to the first
   wildcard, it falls into the reverse-engineered ( by fred1 ) matches
   function.  it's proven to be pretty durn fast.  thanks to justin slootsky,
   who came up with the original idea of running the matches backward ( in
   slightly more complex form ).
   i tried to put lots of comments, cos string matching is not my thing,
   and i had to sit and think for 5 minutes for every line. ( ugh! )
*/
int
wild_match( const char *ma, const char *na  )
{
	const char *mask = ma, *nask = na, *m = ma, *n = na, *mlast;
	int close = 0, q = 0, wild = 0;

	/* take care of null strings ( should never match ) */
	if( ( ma == (char *)0 ) ||
		( na == (char *)0 ) )
		return 0;

	if( ( !*ma ) || ( !*na ) )
		return 0;
	/* find the end of each string */
	while( *m ) m++;
	while( *n ) n++;
	m--;
	n--;
	mlast = m;

	/* check the match backwards */
	/* while:
	   chars are identical OR the mask char is an unquoted '?'
       & haven't reached the start of either string
       & mask char isn't an unquoted '*'
	   */
	while( ( ( tolower( *m ) == tolower( *n ) ) ||
		( ( *m == '?' ) && ( m[ -1 ] != '\\' ) ) ) &&
		( m != ma ) && ( n != na ) &&
		!( ( *m == '*' ) && ( m[ -1 ] != '\\' ) ) )
	{
		if( !( ( *m == '?' ) && ( m[ -1 ] != '\\' ) ) )
			close++; /* 1 more exact match */
		m--;
		n--;
		if( *m == '\\' )
			m--; /* if mask was quoting something, skip the \ */
	}

	if( ( *m  !=  '*' ) || ( *( m - 1 ) == '\\' ) ) {
		/* case II - entire string matched, there were no '*' */
		if( ( m == ma ) && ( n == na ) &&
			( tolower( *m ) == tolower( *n ) ) )
			return close+1;
		/* case III - one of the strings ended prematurely ( no match ) */
		if( ( m == ma ) || ( n == na ) )
			return 0;
		/* case IV - failed to match the ending strings, so abort */
		return 0;
	}
	/* else */

	/* case I - hit an unquoted '*' -- fall into matching algorithm */
	while( 1 ) {
		q = ( m > mask ) ? ( *( m - 1 ) == '\\' ) : 0;  /* quoted? */
		if( m < mask ) {
			if( n < nask )
				return close + 1; /* Made it through both strings! */
			for( m++; ( m < mlast ) ? ( *m == '?' ) : 0; m++ )
				;  /* skip ?s */
			if( ( *m == '*' ) && ( m < mlast ) )
				return close + 1;
			if( wild ) {
				m = ma;
				n = --na;
			}
			else
				return 0;
		}
		else if( n < nask ) {
			while( ( *m == '*' ) && ( m >= mask ) )
				m--;
			return ( m < mask ) ? close+1 : 0;
		}
		q = ( m > mask ) ? ( *( m - 1 ) == '\\' ) : 0; /* quoted? */
		if( ( *m == '*' ) && ( !q ) ) { /* unquoted '*' ? */
			while( ( m > mask ) ? ( *m == '*' ) : 0 )
				m--;  /* Throw away the *s */
			if( *m == '\\' ) {
				m++;
				q = 1;
			}  /* First non-* was quoted, so keep it */
			wild = 1;
			ma = m;
			na = n;
		}
		if( ( tolower( *m )  !=  tolower( *n ) ) &&
			( ( *m  !=  '?' ) || q ) )
			{
				/* non-match */
				if( wild ) {
					m = ma;
					n = --na;
					q=( m > mask ) ? ( *( m - 1 ) == '\\' ) : 0;  /* quoted? */
				}
				else
					return 0;
			}
		else {
			if( ( *m != '?' ) || q )
				close++; /* Unquoted ?s aren't counted */
			if( *m ) m--; /* This char got matched */
			if( *n ) n--; /* This char got matched */
			if( q ) m--; /* This quote went with the char we matched */
		}
	}
}
