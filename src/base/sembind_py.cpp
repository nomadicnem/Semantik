// Thomas Nagy 2007-2020 GPLV3

#include <Python.h>

#include <aux.h>
#include <QFile>
#include <QtDebug>
#include <QByteArray>
#include "sembind.h"

const char* BIND_NODE = "BIND_NODE";

PyObject *from_qstring(const QString &i_s)
{
	QByteArray l_b = i_s.toUtf8();
	PyObject *l_o = PyUnicode_FromStringAndSize(l_b.data(), l_b.size());
	Py_XINCREF(l_o);
	return l_o;
}

QString from_unicode(PyObject* i_o)
{
	if (!i_o)
	{
		return QString();
	}

	int is_string = PyUnicode_Check(i_o);
	if (!is_string)
	{
		//qDebug()<<"not a string!!";
		return QString();
	}

	Py_ssize_t l_iSize = 0;
	const char * l_iChars = const_cast<const char*>(PyUnicode_AsUTF8AndSize(i_o, &l_iSize));

	QString l_s = QString::fromUtf8(l_iChars, l_iSize);
	QChar * data = l_s.data();
	QString ret = QString(data); // force a deep copy, python can safely collect i_o
	return ret;
}

PyObject *Node_instance(PyObject *i_s, PyObject *i_oArgs)
{
	bind_node *l_oFils = bind_node::instance();
	return PyCapsule_New(l_oFils, BIND_NODE, NULL);
}

PyObject *Node_child_num(PyObject *i_s, PyObject *i_oArgs)
{
	PyObject * i_oObj1 = NULL;
	int num = 0;
	if (!PyArg_ParseTuple(i_oArgs, "Oi", &i_oObj1, &num))
	{
		Q_ASSERT(false);
		return NULL;
	}
	bind_node *l_oParent = (bind_node*) PyCapsule_GetPointer(i_oObj1, BIND_NODE);
	Q_ASSERT(l_oParent);
	bind_node *l_oFils = l_oParent->child_num(num);
	Q_ASSERT(l_oFils);
	return PyCapsule_New(l_oFils, BIND_NODE, NULL);
}

PyObject *Node_get_cell(PyObject *i_s, PyObject *i_oArgs)
{
	PyObject * i_oObj1 = NULL;
	int row = 0; int col = 0;
	if (!PyArg_ParseTuple(i_oArgs, "Oii", &i_oObj1, &row, &col))
	{
		Q_ASSERT(false);
		return NULL;
	}
	bind_node *l_oParent = (bind_node*) PyCapsule_GetPointer(i_oObj1, BIND_NODE);
	Q_ASSERT(l_oParent);
	QString content = l_oParent->tbl_cell(row, col);
	return from_qstring(content);
}

PyObject *Node_child_count(PyObject *i_s, PyObject *i_oArgs)
{
	PyObject * i_oObj1 = NULL;
	if (!PyArg_ParseTuple(i_oArgs, "O", &i_oObj1))
	{
		Q_ASSERT(false);
		return NULL;
	}
	bind_node *l_oParent = (bind_node*) PyCapsule_GetPointer(i_oObj1, BIND_NODE);
	int num = l_oParent->child_count();
	return Py_BuildValue("i", num);
}

PyObject *Node_num_rows(PyObject *i_s, PyObject *i_oArgs)
{
	PyObject * i_oObj1 = NULL;
	if (!PyArg_ParseTuple(i_oArgs, "O", &i_oObj1))
	{
		Q_ASSERT(false);
		return NULL;
	}
	bind_node *l_oParent = (bind_node*) PyCapsule_GetPointer(i_oObj1, BIND_NODE);
	int num = l_oParent->num_rows();
	return Py_BuildValue("i", num);
}


PyObject *Node_num_cols(PyObject *i_s, PyObject *i_oArgs)
{
	PyObject * i_oObj1 = NULL;
	if (!PyArg_ParseTuple(i_oArgs, "O", &i_oObj1))
	{
		Q_ASSERT(false);
		return NULL;
	}
	bind_node *l_oParent = (bind_node*) PyCapsule_GetPointer(i_oObj1, BIND_NODE);
	int l_iNum = l_oParent->num_cols();
	return Py_BuildValue("i", l_iNum);
}

PyObject *Node_get_val(PyObject *i_s, PyObject *i_oArgs)
{
	PyObject *i_oObj1 = NULL;
	PyObject *l_oObj2 = NULL;
	if (!PyArg_ParseTuple(i_oArgs, "OO", &i_oObj1, &l_oObj2))
	{
		Q_ASSERT(false);
		return NULL;
	}
	bind_node* l_o = (bind_node*) PyCapsule_GetPointer(i_oObj1, BIND_NODE);
	Q_ASSERT(l_o);
	QString l_sCmd = from_unicode(l_oObj2);
	QString l_s = l_o->get_val(l_sCmd);
	return from_qstring(l_s);
}

PyObject *Node_set_val(PyObject *i_s, PyObject *i_oArgs)
{
	qDebug()<<"set val is not ready";
	PyObject *i_oObj1 = NULL;
	PyObject *l_oObj2 = NULL;
	PyObject *l_oObj3 = NULL;
	if (!PyArg_ParseTuple(i_oArgs, "OOO", &i_oObj1, &l_oObj2, &l_oObj3))
	{
		Q_ASSERT(false);
		return NULL;
	}
	bind_node* l_o = (bind_node*) PyCapsule_GetPointer(i_oObj1, BIND_NODE);
	Q_ASSERT(l_o);
	QString l_sKey = from_unicode(l_oObj2);
	QString val = from_unicode(l_oObj3);
	l_o->set_val(l_sKey, val);
	return Py_None;
}

PyObject *Node_get_var(PyObject *i_s, PyObject *i_oArgs)
{
	PyObject *i_oObj1 = NULL;
	if (!PyArg_ParseTuple(i_oArgs, "O", &i_oObj1))
	{
		Q_ASSERT(false);
		return NULL;
	}
	QString l_sCmd = from_unicode(i_oObj1);
	QString l_s = bind_node::get_var(l_sCmd);
	return from_qstring(l_s);
}

PyObject *Node_set_var(PyObject *i_s, PyObject *i_oArgs)
{
	PyObject *i_oObj1 = NULL;
	PyObject *l_oObj2 = NULL;
	if (!PyArg_ParseTuple(i_oArgs, "OO", &i_oObj1, &l_oObj2))
	{
		Q_ASSERT(false);
		return NULL;
	}
	QString l_sKey = from_unicode(i_oObj1);
	QString l_sVal = from_unicode(l_oObj2);

	bind_node::set_var(l_sKey, l_sVal);
	return Py_None;
}

PyObject *Node_set_result(PyObject *i_s, PyObject *i_oArgs)
{
	PyObject *i_oObj1 = NULL;
	PyObject *l_oObj2 = NULL;
	if (!PyArg_ParseTuple(i_oArgs, "OO", &i_oObj1, &l_oObj2))
	{
		Q_ASSERT(false);
		return NULL;
	}
	QString l_sKey = from_unicode(i_oObj1);
	QString l_sVal = from_unicode(l_oObj2);

	bind_node::set_result(l_sKey, l_sVal);
	return Py_None;
}

PyObject *Node_protectXML(PyObject *i_s, PyObject *i_oArgs)
{
	PyObject *i_oObj1 = NULL;
	if (!PyArg_ParseTuple(i_oArgs, "O", &i_oObj1))
	{
		Q_ASSERT(false);
		return NULL;
	}
	QString l_sKey = from_unicode(i_oObj1);
	QString l_sRet = bind_node::protectXML(l_sKey);
	return from_qstring(l_sRet);
}

PyObject *Node_protectHTML(PyObject *i_s, PyObject *i_oArgs)
{
	PyObject *i_oObj1 = NULL;
	if (!PyArg_ParseTuple(i_oArgs, "O", &i_oObj1))
	{
		Q_ASSERT(false);
		return NULL;
	}
	QString l_sKey = from_unicode(i_oObj1);
	QString l_sRet = bind_node::protectHTML(l_sKey);
	return from_qstring(l_sRet);
}

PyObject *Node_get_item_ids(PyObject *i_s, PyObject *i_oArgs)
{
	QString ids = bind_node::get_item_ids();
	return from_qstring(ids);
}

PyObject *Node_get_item_by_id(PyObject *i_s, PyObject *i_oArgs)
{
	int id = 0;
	if (!PyArg_ParseTuple(i_oArgs, "i", &id))
	{
		Q_ASSERT(false);
		return NULL;
	}
	bind_node *l_oFils = bind_node::get_item_by_id(id);
	Q_ASSERT(l_oFils);
        return PyCapsule_New(l_oFils, BIND_NODE, NULL);
}

static PyMethodDef methods[] = {
	// node methods
	{"child_count", Node_child_count, METH_VARARGS, NULL},
	{"child_num",   Node_child_num, METH_VARARGS, NULL},
	{"get_val",   Node_get_val, METH_VARARGS, NULL},
	{"set_val",   Node_set_val, METH_VARARGS, NULL},

	// cells
	{"num_rows", Node_num_rows, METH_VARARGS, NULL},
	{"num_cols", Node_num_cols, METH_VARARGS, NULL},
	{"get_cell", Node_get_cell, METH_VARARGS, NULL},

	// static methods
	{"instance", Node_instance, METH_VARARGS, NULL},
	{"get_var",  Node_get_var, METH_VARARGS, NULL},
	{"set_var",  Node_set_var, METH_VARARGS, NULL},

	{"set_result",  Node_set_result, METH_VARARGS, NULL},

	{"get_item_ids", Node_get_item_ids, METH_VARARGS, NULL},
	{"get_item_by_id", Node_get_item_by_id, METH_VARARGS, NULL},

	// utilities
	{"protectXML", Node_protectXML, METH_VARARGS, NULL},
	{"protectHTML", Node_protectHTML, METH_VARARGS, NULL},

	{ NULL, NULL, 0, NULL }
};

static PyModuleDef sembind_module = {
	PyModuleDef_HEAD_INIT,
	"sembind",
	NULL,
	-1,
	methods,
	NULL,
	NULL,
	NULL,
	NULL
};

static PyObject* PyInit_sembind(void)
{
	return PyModule_Create(&sembind_module);
}

int init_py()
{
	if (Py_IsInitialized())
	{
		return 1;
	}
	PyImport_AppendInittab("sembind", &PyInit_sembind);
	Py_Initialize();

	QFile l_o(SEMANTIK_DIR "/sembind.py");
	if (!l_o.open(QIODevice::ReadOnly)) { return 0; }
	QByteArray l_oBa = l_o.readAll();
	l_o.close();

	int ret = PyRun_SimpleString(l_oBa.constData());
	if (ret != 0)
	{
		return 0;
	}
	return 1;
}

