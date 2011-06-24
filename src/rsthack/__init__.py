"""
ReStructuredText Hack
=====================

:author: Forrest Y. Yu <forrest.yu@gmail.com>, http://forrestyu.net/

directive ``colorcode``
-----------------------

.. colorcode:: c

	int main()
	{
		printf("Hello world!\n");
		return 0;
	}

role ``mil``
------------

(``mil`` stands for math-inline)

foo :mil:`W \approx \sum{f(x_k) \Delta x}` bar

directive ``math``
------------------

.. math::

    \frac{1}{\Bigl(\sqrt{\phi \sqrt{5}}-\phi\Bigr) e^{\frac25 \pi}} =
    1+\frac{e^{-2\pi}} {1+\frac{e^{-4\pi}} {1+\frac{e^{-6\pi}}
    {1+\frac{e^{-8\pi}} {1+\ldots} } } }

.. math::
	:number: 3
	:label: fibo

	F_{n}=\begin{cases}
	0 & n=0\\
	1 & n=1\\
	F_{n-1}+F_{n-2} & n>1
	\end{cases}

Equation :eq:`fibo` is good.

"""

import re

from docutils import nodes, utils
from docutils.parsers.rst import directives, Directive, roles
from docutils.core import publish_doctree, publish_from_doctree, Publisher
from docutils.io import FileInput
from docutils.writers.html4css1 import HTMLTranslator as translator

from pygments.formatters import HtmlFormatter
from pygments import highlight
from pygments.lexers import get_lexer_by_name, TextLexer

class_name = 'mathjaxlatex'  # appears in <span class="mathjaxlatex"> and <div class="mathjaxlatex">

######################################################################
# directive colorcode
######################################################################

# REF: http://docutils.sourceforge.net/docs/howto/rst-directives.html

# Set to True if you want inline CSS styles instead of classes
INLINESTYLES = True

# Add name -> formatter pairs for every variant you want to use
# http://pygments.org/docs/formatters/
VARIANTS = {
    #'linenos': HtmlFormatter(noclasses=INLINESTYLES, linenos='inline', nobackground=True),
}

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
# role mil
######################################################################

def MRole(role, rawtext, text, lineno, inliner, options={}, content=[]):
    assert role == 'mil'
    t = re.sub(':%s:' % role, '', rawtext)
    assert t[:1] == '`' and t[-1:] == '`'
    t = r'\(%s\)' % t[1:-1]

    # options are hard-coded, since there's nowhere to place them:
    options={'format': 'html', 'classes': [class_name]}

    node = nodes.raw(rawtext, t, **options)

    # this works too:
    # node = nodes.inline(rawtext, t, **options)

    return [node], []

roles.register_canonical_role('mil', MRole)

######################################################################
# role eq
######################################################################
# see: /usr/share/pyshared/sphinx/ext/mathbase.py

class eqref(nodes.Inline, nodes.TextElement):
    pass

def html_visit_eqref(self, node):
    self.body.append('<a href="#equation-%s">' % node['target'])

def html_depart_eqref(self, node):
    self.body.append('</a>')

def eq_role(role, rawtext, text, lineno, inliner, options={}, content=[]):
    text = utils.unescape(text)
    node = eqref('(?)', '(?)', target=text)
    return [node], []

roles.register_canonical_role('eq', eq_role)

######################################################################
# directive math
######################################################################
# see:
#     /usr/share/pyshared/sphinx/application.py
#     /usr/share/pyshared/sphinx/ext/mathbase.py
#     /usr/share/pyshared/sphinx/ext/jsmath.py
#     /usr/share/doc/python-sphinx/html/ext/math.html

def massage_equations(doctree):
    num = 0
    numbers = {}

    # generate the "label->number" map from directive ``math``
    for node in doctree.traverse(displaymath):
        if node['number'] is None:
            continue
        numbers[node['label']] = node['number']
    #print >> sys.stderr, 'label->number map: ', numbers

    # set eqno in references
    for node in doctree.traverse(eqref):
        if node['target'] not in numbers:
            continue
        num = '(%d)' % numbers[node['target']]
        node[0] = nodes.Text(num, num)

def html_visit_displaymath(self, node):
    if node['nowrap']:
        self.body.append(self.starttag(node, 'div', CLASS=class_name))
        self.body.append(node['latex'])
        self.body.append('</div>')
        raise nodes.SkipNode
    for i, part in enumerate(node['latex'].split('\n\n')):
        part = self.encode(part)
        if i == 0:
            # necessary to e.g. set the id property correctly
            if node['number'] is not None:
                self.body.append('<span class="eqno">(%s)</span>' %
                                 node['number'])
            self.body.append(self.starttag(node, 'div', CLASS=class_name))
        else:
            # but only once!
            self.body.append('<div class="%s">' % class_name)
        if '&' in part or '\\\\' in part:
            self.body.append('\\[\\begin{align}\n' + part + '\n\\end{align}\\]')
        else:
            self.body.append('\\[' + part + '\\]')
        self.body.append('</div>\n')
    raise nodes.SkipNode

def add_node(node, **kwds):
    nodes._add_node_class_names([node.__name__])
    for key, val in kwds.iteritems():
        try:
            visit, depart = val
        except ValueError:
            raise ExtensionError('Value for key %r must be a '
                                 '(visit, depart) function tuple' % key)

        assert key == 'html', 'accept html only'

        setattr(translator, 'visit_'+node.__name__, visit)
        if depart:
            setattr(translator, 'depart_'+node.__name__, depart)

class displaymath(nodes.Part, nodes.Element):
    pass

# custom directive ``math``
class MathDisplay(Directive):
    has_content = True
    required_arguments = 0
    optional_arguments = 1
    final_argument_whitespace = True
    option_spec = {
        'label':  directives.unchanged,
        'nowrap': directives.flag,
        'number': directives.nonnegative_int,
    }

    def run(self):
        latex = '\n'.join(self.content)
        if self.arguments and self.arguments[0]:
	    latex = self.arguments[0] + '\n\n' + latex
        node = displaymath()
        node['latex']  = latex
        node['number'] = self.options.get('number', None)
        node['label']  = self.options.get('label', node['number'])
        node['nowrap'] = 'nowrap' in self.options

	#print >> sys.stderr, 'number: ', node['number'], 'label: ', node['label']

        ret = [node]

        if node['label']:
            tnode = nodes.target('', '', ids=['equation-' + node['label']])
            self.state.document.note_explicit_target(tnode)
            ret.insert(0, tnode)
        return ret

add_node(eqref, html=(html_visit_eqref, html_depart_eqref))
add_node(displaymath, html=(html_visit_displaymath, None))

directives.register_directive('math', MathDisplay)
