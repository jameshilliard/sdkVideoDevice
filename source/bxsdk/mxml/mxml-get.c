/*
 * "$Id: mxml-get.c 427 2011-01-03 02:03:29Z mike $"
 *
 * Node get functions for Mini-XML, a small XML-like file parsing library.
 *
 * Copyright 2011 by Michael R Sweet.
 *
 * These coded instructions, statements, and computer programs are the
 * property of Michael R Sweet and are protected by Federal copyright
 * law.  Distribution and use rights are outlined in the file "COPYING"
 * which should have been included with this file.  If this file is
 * missing or damaged, see the license at:
 *
 *     http://www.minixml.org/
 *
 * Contents:
 *
 *   mxmlGetCDATA()       - Get the value for a CDATA node.
 *   mxmlGetCustom()      - Get the value for a custom node.
 *   mxmlGetElement()     - Get the name for an element node.
 *   mxmlGetFirstChild()  - Get the first child of an element node.
 *   mxmlGetInteger()     - Get the integer value from the specified node or its
 *                          first child.
 *   mxmlGetLastChild()   - Get the last child of an element node.
 *   mxmlGetNextSibling() - Get the next node for the current parent.
 *   mxmlGetOpaque()      - Get an opaque string value for a node or its first
 *                          child.
 *   mxmlGetParent()      - Get the parent node.
 *   mxmlGetPrevSibling() - Get the previous node for the current parent.
 *   mxmlGetReal()        - Get the real value for a node or its first child.
 *   mxmlGetText()        - Get the text value for a node or its first child.
 *   mxmlGetType()        - Get the node type.
 *   mxmlGetUserData()    - Get the user data pointer for a node.
 */

/*
 * Include necessary headers...
 */

#include "config.h"
#include "mxml.h"


/*
 * 'mxmlGetCDATA()' - Get the value for a CDATA node.
 *
 * @code NULL@ is returned if the node is not a CDATA element.
 *
 * @since Mini-XML 2.7@
 */

const char *				/* O - CDATA value or NULL */
SZY_mxmlGetCDATA(mxml_node_t *node)		/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node || node->type != MXML_ELEMENT ||
      strncmp(node->value.element.name, "![CDATA[", 8))
    return (NULL);

 /*
  * Return the text following the CDATA declaration...
  */

  return (node->value.element.name + 8);
}


/*
 * 'mxmlGetCustom()' - Get the value for a custom node.
 *
 * @code NULL@ is returned if the node (or its first child) is not a custom
 * value node.
 *
 * @since Mini-XML 2.7@
 */

const void *				/* O - Custom value or NULL */
SZY_mxmlGetCustom(mxml_node_t *node)	/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node)
    return (NULL);

 /*
  * Return the integer value...
  */

  if (node->type == MXML_CUSTOM)
    return (node->value.custom.data);
  else if (node->type == MXML_ELEMENT &&
           node->child &&
	   node->child->type == MXML_CUSTOM)
    return (node->child->value.custom.data);
  else
    return (NULL);
}


/*
 * 'mxmlGetElement()' - Get the name for an element node.
 *
 * @code NULL@ is returned if the node is not an element node.
 *
 * @since Mini-XML 2.7@
 */

const char *				/* O - Element name or NULL */
SZY_mxmlGetElement(mxml_node_t *node)	/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node || node->type != MXML_ELEMENT)
    return (NULL);

 /*
  * Return the element name...
  */

  return (node->value.element.name);
}


/*
 * 'mxmlGetFirstChild()' - Get the first child of an element node.
 *
 * @code NULL@ is returned if the node is not an element node or if the node
 * has no children.
 *
 * @since Mini-XML 2.7@
 */

mxml_node_t *				/* O - First child or NULL */
SZY_mxmlGetFirstChild(mxml_node_t *node)	/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node || node->type != MXML_ELEMENT)
    return (NULL);

 /*
  * Return the first child node...
  */

  return (node->child);
}


/*
 * 'mxmlGetInteger()' - Get the integer value from the specified node or its
 *                      first child.
 *
 * 0 is returned if the node (or its first child) is not an integer value node.
 *
 * @since Mini-XML 2.7@
 */

int					/* O - Integer value or 0 */
SZY_mxmlGetInteger(mxml_node_t *node)	/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node)
    return (0);

 /*
  * Return the integer value...
  */

  if (node->type == MXML_INTEGER)
    return (node->value.integer);
  else if (node->type == MXML_ELEMENT &&
           node->child &&
	   node->child->type == MXML_INTEGER)
    return (node->child->value.integer);
  else
    return (0);
}


/*
 * 'mxmlGetLastChild()' - Get the last child of an element node.
 *
 * @code NULL@ is returned if the node is not an element node or if the node
 * has no children.
 *
 * @since Mini-XML 2.7@
 */

mxml_node_t *				/* O - Last child or NULL */
SZY_mxmlGetLastChild(mxml_node_t *node)	/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node || node->type != MXML_ELEMENT)
    return (NULL);

 /*
  * Return the node type...
  */

  return (node->last_child);
}


/*
 * 'mxmlGetNextSibling()' - Get the next node for the current parent.
 *
 * @code NULL@ is returned if this is the last child for the current parent.
 *
 * @since Mini-XML 2.7@
 */

mxml_node_t *
SZY_mxmlGetNextSibling(mxml_node_t *node)	/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node)
    return (NULL);

 /*
  * Return the node type...
  */

  return (node->next);
}


/*
 * 'mxmlGetOpaque()' - Get an opaque string value for a node or its first child.
 *
 * @code NULL@ is returned if the node (or its first child) is not an opaque
 * value node.
 *
 * @since Mini-XML 2.7@
 */

const char *				/* O - Opaque string or NULL */
SZY_mxmlGetOpaque(mxml_node_t *node)	/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node)
    return (NULL);

 /*
  * Return the integer value...
  */

  if (node->type == MXML_OPAQUE)
    return (node->value.opaque);
  else if (node->type == MXML_ELEMENT &&
           node->child &&
	   node->child->type == MXML_OPAQUE)
    return (node->child->value.opaque);
  else
    return (NULL);
}


/*
 * 'mxmlGetParent()' - Get the parent node.
 *
 * @code NULL@ is returned for a root node.
 *
 * @since Mini-XML 2.7@
 */

mxml_node_t *				/* O - Parent node or NULL */
SZY_mxmlGetParent(mxml_node_t *node)	/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node)
    return (NULL);

 /*
  * Return the node type...
  */

  return (node->parent);
}


/*
 * 'mxmlGetPrevSibling()' - Get the previous node for the current parent.
 *
 * @code NULL@ is returned if this is the first child for the current parent.
 *
 * @since Mini-XML 2.7@
 */

mxml_node_t *				/* O - Previous node or NULL */
SZY_mxmlGetPrevSibling(mxml_node_t *node)	/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node)
    return (NULL);

 /*
  * Return the node type...
  */

  return (node->prev);
}


/*
 * 'mxmlGetReal()' - Get the real value for a node or its first child.
 *
 * 0.0 is returned if the node (or its first child) is not a real value node.
 *
 * @since Mini-XML 2.7@
 */

double					/* O - Real value or 0.0 */
SZY_mxmlGetReal(mxml_node_t *node)		/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node)
    return (0.0);

 /*
  * Return the integer value...
  */

  if (node->type == MXML_REAL)
    return (node->value.real);
  else if (node->type == MXML_ELEMENT &&
           node->child &&
	   node->child->type == MXML_REAL)
    return (node->child->value.real);
  else
    return (0.0);
}


/*
 * 'mxmlGetText()' - Get the text value for a node or its first child.
 *
 * @code NULL@ is returned if the node (or its first child) is not a text node.
 * The "whitespace" argument can be NULL.
 *
 * @since Mini-XML 2.7@
 */

const char *				/* O - Text string or NULL */
SZY_mxmlGetText(mxml_node_t *node,		/* I - Node to get */
            int         *whitespace)	/* O - 1 if string is preceded by whitespace, 0 otherwise */
{
 /*
  * Range check input...
  */

  if (!node)
  {
    if (whitespace)
      *whitespace = 0;

    return (NULL);
  }

 /*
  * Return the integer value...
  */

  if (node->type == MXML_TEXT)
  {
    if (whitespace)
      *whitespace = node->value.text.whitespace;

    return (node->value.text.string);
  }
  else if (node->type == MXML_ELEMENT &&
           node->child &&
	   node->child->type == MXML_TEXT)
  {
    if (whitespace)
      *whitespace = node->child->value.text.whitespace;

    return (node->child->value.text.string);
  }
  else
  {
    if (whitespace)
      *whitespace = 0;

    return (NULL);
  }
}


/*
 * 'mxmlGetType()' - Get the node type.
 *
 * @code MXML_IGNORE@ is returned if "node" is @code NULL@.
 *
 * @since Mini-XML 2.7@
 */

mxml_type_t				/* O - Type of node */
SZY_mxmlGetType(mxml_node_t *node)		/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node)
    return (MXML_IGNORE);

 /*
  * Return the node type...
  */

  return (node->type);
}


/*
 * 'mxmlGetUserData()' - Get the user data pointer for a node.
 *
 * @since Mini-XML 2.7@
 */

void *					/* O - User data pointer */
SZY_mxmlGetUserData(mxml_node_t *node)	/* I - Node to get */
{
 /*
  * Range check input...
  */

  if (!node)
    return (NULL);

 /*
  * Return the user data pointer...
  */

  return (node->user_data);
}


/*
 * End of "$Id: mxml-get.c 427 2011-01-03 02:03:29Z mike $".
 */
