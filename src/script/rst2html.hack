#!/usr/bin/python

# $Id: rst2html.py 4564 2006-05-21 20:44:42Z wiemann $
# Author: David Goodger <goodger@python.org>
# Copyright: This module has been placed in the public domain.

"""
A minimal front end to the Docutils Publisher, producing HTML.
Hacked by Forrest Y. Yu <forrest.yu@gmail.com> @ Jun.2011

One more directive ``colorcode`` is added::

	.. colorcode:: c

		int main()
		{
			printf("Hello world!\n");
			return 0;
		}

"""

import sys
print >> sys.stderr, '\033[32m' + 'hacked' + '\033[0;36m' + ' rst2html' + '\033[0m'

######################################################################
# Pygments support
######################################################################

# REF: http://docutils.sourceforge.net/docs/howto/rst-directives.html

# Set to True if you want inline CSS styles instead of classes
INLINESTYLES = True

from pygments.formatters import HtmlFormatter

# Add name -> formatter pairs for every variant you want to use
# http://pygments.org/docs/formatters/
VARIANTS = {
    #'linenos': HtmlFormatter(noclasses=INLINESTYLES, linenos='inline', nobackground=True),
}

from docutils import nodes
from docutils.parsers.rst import directives, Directive

from pygments import highlight
from pygments.lexers import get_lexer_by_name, TextLexer

class Pygments(Directive):
    """ Source code syntax hightlighting.
    """
    required_arguments = 1
    optional_arguments = 0
    final_argument_whitespace = True

    option_spec = {'linenos'     : directives.flag,
		    'lineanchors' : lambda x: 'code-%s' % x
		   #'lineanchors' : directives.unchanged_required,  # see /usr/share/pyshared/docutils/parsers/rst/directives/__init__.py
		  }

    has_content = True

    def run(self):
        self.assert_has_content()
        try:
            lexer = get_lexer_by_name(self.arguments[0])
        except ValueError:
            # no lexer found - use the text one instead of an exception
            lexer = TextLexer()

	if self.options:
            if self.options.has_key('linenos'):
	        if self.options.has_key('lineanchors'):
                    formatter = HtmlFormatter(noclasses=INLINESTYLES, linenos='table', nobackground=True, anchorlinenos=True, lineanchors=self.options['lineanchors'])
                else:
                    formatter = HtmlFormatter(noclasses=INLINESTYLES, linenos='table', nobackground=True)
            else:
                formatter = VARIANTS[self.options.keys()[0]]
	else:
            formatter = HtmlFormatter(noclasses=INLINESTYLES, nobackground=True)

        parsed = highlight(u'\n'.join(self.content), lexer, formatter)
        return [nodes.raw('', parsed, format='html')]

directives.register_directive('colorcode', Pygments)

######################################################################
# original rst2html:
######################################################################

try:
    import locale
    locale.setlocale(locale.LC_ALL, '')
except:
    pass

from docutils.core import publish_cmdline, default_description


description = ('Generates (X)HTML documents from standalone reStructuredText '
               'sources.  ' + default_description)

publish_cmdline(writer_name='html', description=description)
