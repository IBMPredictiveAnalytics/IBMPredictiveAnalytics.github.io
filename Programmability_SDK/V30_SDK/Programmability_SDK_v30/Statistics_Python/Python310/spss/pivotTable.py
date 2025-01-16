#/***********************************************************************
# * Licensed Materials - Property of IBM 
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2015
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp. 
# ************************************************************************/

#!/usr/bin/python
"""Create pivot table
"""

#Copyright (C) 2008 by SPSS Inc.

import spss.spss
from . import PyInvokeSpss
from .errMsg import *
from .spss import error
from . import FormatSpec
from . import spssutil
import time
import datetime
import operator

class BasePivotTable(object):
    def __init__(self,title,templateName,outline="",isSplit=True,caption=""):
        """
        Creates the structure of a pivot table.

        --usage
          BasePivotTable(title,templateName,outline="",isSplit=True,caption="")

        --arguments
          title: The string title for the pivot table.
          templateName: The string OMS table subtype for the table.
          outline: An optional argument that specifies a string outline title for the table.
          isSplit: An optional boolean value to indicate whether or not to enable split file processing for the table.
          caption: An optional argument that specifies a string caption for the table.

    --details
          If the optional outline argument is not specified, the table will be placed one level under the root item
          for the procedure that created it. Otherwise, the table will be placed under the specified outline title.
        """

        error.Reset()
        #Checks the template name. It should start with a letter and has the max length 64 bytes.
        if len(templateName) < 1 or len(templateName) > 64:
            error.SetErrorCode(1036)
            if error.IsError():
                raise SpssError(error)
        if not templateName[0].isalpha():
            error.SetErrorCode(1036)
            if error.IsError():
                raise SpssError(error)

        self.outline = outline
        self.title = title
        self.templateName = templateName
        self.isSplit = isSplit
        self.dims = []
        self.cells = {}
        self.caption = caption

        #Creates the structure of the pivot table.
        error.SetErrorCode(PyInvokeSpss.StartPivotTable(spssutil.CheckStr(self.outline),
                                                        spssutil.CheckStr(self.title),
                                                        spssutil.CheckStr(self.templateName),
                                                        self.isSplit))
        if error.IsError():
            raise SpssError(error)

        #Adds caption into the pivot table is needed.
        if self.caption:
            self.Caption(caption)

        self.SetDefaultFormatSpec(FormatSpec.GeneralStat)

    def __contains__(self, dim):
        """
        Reports if the pivot table has a dimension "dim".

        --arguments
          dim: must be an instance of class Dimension.
        """
        return dim in self.dims

    def __len__(self):
        """
        Returns the number of dimensions in the pivot table.
        """
        return len(self.dims)

    def __iter__(self):
        """
        Iterator based on Dimension.
        """
        for dim in self.dims:
            yield dim

    def __repr__(self):
        return repr(self.title)

    def __getitem__(self, categories):
        """
        Returns the cell object indicated by a tuple or list of categories.

        --usage
          cell = table[categories]

        --arguments
          categories: A tuple or list of categories. Each element must be
                          CellText.Number,
                          CellText.String,
                          CellText.VarName, or
                          CellText.VarValue.

        --example
            import spss
            try:
                spss.StartSPSS()
                spss.Submit("get file='demo.sav'.")
                spss.StartProcedure("proc")
                table = spss.BasePivotTable("table","mytable")
                table.Append(spss.Dimension.Place.row,"rowdim")
                table.Append(spss.Dimension.Place.column,"coldim")

                category1 = spss.CellText.String("M")
                category2 = spss.CellText.String("N")
                category3 = spss.CellText.String("W")

                table[(category1,category2)] = spss.CellText.Number(23,spss.FormatSpec.Count)
                cell = table[[category1,category2]]

                table[(category1,category3)] = cell

                spss.EndProcedure()
                spss.StopSPSS()
            except spss.SpssError:
              print("Error.")
            print(spss.GetLastErrorMessage())
        """
        error.Reset()
        if not isinstance(categories, (tuple,list)):
            error.SetErrorCode(1027)
            if error.IsError():
                raise SpssError(error)

        #The categories must match with the structure of the dimensions. One for each Dimension.
        if len(categories) != len(self.dims):
            error.SetErrorCode(1038)
            if error.IsError():
                raise SpssError(error)

        #Each element in the categories must be a object of CellText.
        list(map(CellText._CellText__CheckType,categories))

        if tuple(categories) in self.cells:
            return self.cells[tuple(categories)]
 
        else:
            for key in list(self.cells.keys()):
                find = True
                for (i,x) in enumerate(key):
                    if x != categories[i]:
                        find = False
                    if not find:
                        break
                if find:
                    return self.cells[key]
            return None

    def __setitem__(self, categories, cell):
        """
        Sets the cell object indicated by a tuple or list of categories.

        --usage
          table[categories] = cell

        --arguments
          categories: A tuple or list of categories. Each element must be
                          CellText.Number,
                          CellText.String,
                          CellText.VarName, or
                          CellText.VarValue.
          cell: The cell value. Must be one of
                          CellText.Number,
                          CellText.String,
                          CellText.VarName, or
                          CellText.VarValue.

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc")
              table = spss.BasePivotTable("table","mytable")
              table.Append(spss.Dimension.Place.row,"rowdim")
              table.Append(spss.Dimension.Place.column,"coldim")
              table[(spss.CellText.String("M"),spss.CellText.String("N"))] = spss.CellText.Number(23,spss.FormatSpec.Count)
              spss.EndProcedure()
              spss.StopSPSS()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
        """
        error.Reset()

        if not isinstance(categories, (tuple,list)):
            error.SetErrorCode(1027)
            if error.IsError():
                raise SpssError(error)

        #The categories must match with the structure of the dimensions. One for each dimension.
        if len(categories) != len(self.dims):
            error.SetErrorCode(1038)
            if error.IsError():
                raise SpssError(error)

        #Each element in the categories must be a object of CellText.
        list(map(CellText._CellText__CheckType,categories))

        #Adds the categories to the pivot table.
        for (i, dim) in enumerate(self.dims):
            dim._Dimension__SetCategory(categories[i])

        if cell is not None:
            #The given cell must be a object of CellText.
            CellText._CellText__CheckType(cell)
            #Adds the cell to the pivot table.
            self.dims[-1]._Dimension__SetCell(cell)

        #Saves the cell for retrieve.
        self.cells[tuple(categories)] = cell

    def __delitem__(self, dim):
        """
        Disable __delitem__.
        """
        error.SetErrorCode(1040)
        if error.IsError():
            raise SpssError(error)

    def Footnotes(self, categories, footnotes):
        """
        Adds footnotes to the cell object indicated by a tuple or list of categories.

        --usage
          Footnotes(categories, footnotes)

        --arguments
          categories: A tuple or list of categories. Each element must be:
                          CellText.Number,
                          CellText.String,
                          CellText.VarName, or
                          CellText.VarValue.
          footnotes: The string footnote for the cell.

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc")
              table = spss.BasePivotTable("table","mytable")
              table.Append(spss.Dimension.Place.row,"rowdim")
              table.Append(spss.Dimension.Place.column,"coldim")
              table[(spss.CellText.String("M"),spss.CellText.String("N"))] = spss.CellText.Number(23,spss.FormatSpec.Count)
              table.Footnotes((spss.CellText.String("M"),spss.CellText.String("N")),"My Footnotes")
              spss.EndProcedure()
              spss.StopSPSS()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
        """
        error.Reset()
        if not isinstance(categories, (tuple,list)):
            error.SetErrorCode(1027)
            if error.IsError():
                raise SpssError(error)

        #The categories must match with the structure of the dimensions. One for each dimension.
        if len(categories) != len(self.dims):
            error.SetErrorCode(1038)
            if error.IsError():
                raise SpssError(error)

        #Each element in the categories must be a object of CellText.
        list(map(CellText._CellText__CheckType,categories))

        cell = self[categories]
        if not cell:
            error.SetErrorCode(1041)
            if error.IsError():
                raise SpssError(error)

        #Adds the footnotes to the pivot table.
        for (i, dim) in enumerate(self.dims):
            dim._Dimension__SetCategory(categories[i])
        self.dims[-1]._Dimension__SetCell(cell)
        self.dims[-1]._Dimension__AddFootnotes(footnotes)

    def TitleFootnotes(self, footnotes):
        """
        Adds footnotes to the table title.

        --usage
          TitleFootnotes(footnotes)

        --arguments
          footnotes: The string footnote for the table title.

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc")
              table = spss.BasePivotTable("table","mytable")
              table.TitleFootnotes("My title footnotes")
              table.Append(spss.Dimension.Place.row,"rowdim")
              table.Append(spss.Dimension.Place.column,"coldim")
              table[(spss.CellText.String("M"),spss.CellText.String("N"))] = spss.CellText.Number(23,spss.FormatSpec.Count)
              spss.EndProcedure()
              spss.StopSPSS()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
        """
        error.Reset()
        
        #Adds the footnotes to the pivot table.
        error.SetErrorCode(PyInvokeSpss.AddTitleFootnotes(spssutil.CheckStr(self.outline),
                                                          spssutil.CheckStr(self.title),
                                                          spssutil.CheckStr(self.templateName),
                                                          spssutil.CheckStr(footnotes),
                                                          self.isSplit))
        if error.IsError():
            raise SpssError(error)

    def OutlineFootnotes(self, footnotes):
        """
        Adds footnotes to the table outline.

        --usage
          OutlineFootnotes(footnotes)

        --arguments
          footnotes: The string footnote for the table outline.

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc")
              table = spss.BasePivotTable("table","mytable")
              table.OutlineFootnotes("My outline footnotes")
              table.Append(spss.Dimension.Place.row,"rowdim")
              table.Append(spss.Dimension.Place.column,"coldim")
              table[(spss.CellText.String("M"),spss.CellText.String("N"))] = spss.CellText.Number(23,spss.FormatSpec.Count)
              spss.EndProcedure()
              spss.StopSPSS()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
        """
        error.Reset()
        #Adds the footnotes to the pivot table.
        error.SetErrorCode(PyInvokeSpss.AddOutlineFootnotes(spssutil.CheckStr(self.outline),
                                                            spssutil.CheckStr(self.title),
                                                            spssutil.CheckStr(self.templateName),
                                                            spssutil.CheckStr(footnotes),
                                                            self.isSplit))
        if error.IsError():
            raise SpssError(error)

    def DimensionFootnotes(self, dimPlace, dimName, footnotes):
        """
        Adds footnotes to the dimension indicated by the dimension name.

        --usage
          DimensionFootnotes(dimPlace, dimName, footnotes)

        --arguments
          dimPlace: The dimension place -- row, column, or layer.
          dimName: The string dimension name.
          footnotes: The string footnote for the dimension.

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc")
              table = spss.BasePivotTable("table","mytable")
              table.Append(spss.Dimension.Place.row,"rowdim")
              table.Append(spss.Dimension.Place.column,"coldim")
              table.DimensionFootnotes(spss.Dimension.Place.row,"rowdim","row dimension footnotes")
              table[(spss.CellText.String("M"),spss.CellText.String("N"))] = spss.CellText.Number(23,spss.FormatSpec.Count)
              spss.EndProcedure()
              spss.StopSPSS()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
        """
        error.Reset()
        #Adds the footnotes to the pivot table.
        invalidDim = True
        for (i, dim) in enumerate(self.dims):
            if( dimName == dim.name and dimPlace == dim.place):
                dim._Dimension__AddDimFootnotes(footnotes)
                invalidDim = False
                break

        if invalidDim:
            Dimension._Dimension__CheckPlace(dimPlace)
            error.SetErrorCode(1043)
            if error.IsError():
                raise SpssError(error)
           

    def CategoryFootnotes(self, dimPlace, dimName, category, footnotes):
        """
        Adds footnotes to the category.

        --usage
          CategoryFootnotes(dimPlace, dimName, category, footnotes)

        --arguments
          dimPlace: The dimension place -- row, column, or layer.
          dimName: The string dimension name.
          category: The string category name.
          footnotes: The string footnote for the category.

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc")
              table = spss.BasePivotTable("table","mytable")
              table.Append(spss.Dimension.Place.row,"rowdim")
              table.Append(spss.Dimension.Place.column,"coldim")
              cat1 = spss.CellText.String("M")
              cat2 = spss.CellText.String("N")
              table.CategoryFootnotes(spss.Dimension.Place.row,"rowdim",cat1,"row category1 footnotes")
              table[(cat1,cat2)] = spss.CellText.Number(23,spss.FormatSpec.Count)
              spss.EndProcedure()
              spss.StopSPSS()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
        """
        error.Reset()
        invalidDim = True
        #Adds the footnotes to the pivot table.
        for (i, dim) in enumerate(self.dims):
            if( dimName == dim.name and dimPlace == dim.place):
                dim._Dimension__SetCategory(category)
                dim._Dimension__AddCategoryFootnotes(footnotes)
                invalidDim = False
                break

        if invalidDim:
            Dimension._Dimension__CheckPlace(dimPlace)
            error.SetErrorCode(1043)
            if error.IsError():
                raise SpssError(error)

    def Caption(self,caption):
        """
        Adds caption to the pivot table.

        --usage
          Caption(caption)

        --arguments
          caption: The string caption for the table.

        --example
            import spss
            try:
                spss.StartSPSS()
                spss.Submit("get file='demo.sav'.")
                spss.StartProcedure("proc")
                table = spss.BasePivotTable("table","mytable")
                table.Append(spss.Dimension.Place.row,"rowdim")
                table.Append(spss.Dimension.Place.column,"coldim")
                table[(spss.CellText.String("M"),spss.CellText.String("N"))] = spss.CellText.Number(23,spss.FormatSpec.Count)
                table.Caption("My Caption.")
                spss.EndProcedure()
                spss.StopSPSS()
            except spss.SpssError:
                print("Error.")
            print(spss.GetLastErrorMessage())
        """
        error.Reset()
        self.caption = caption
        error.SetErrorCode(PyInvokeSpss.PivotTableCaption(spssutil.CheckStr(self.outline),
                                                          spssutil.CheckStr(self.title),
                                                          spssutil.CheckStr(self.templateName),
                                                          self.isSplit,
                                                          spssutil.CheckStr(caption)))
        if error.IsError():
            raise SpssError(error)

    def SimplePivotTable(self,rowdim="",rowlabels=[],coldim="",collabels=[],cells=None):
        """
        Creates a simple pivot table by giving data as whole.
        A simple pivot table means there is only one row dimension and one column dimension, no group, no layer.

        --usage
          SimplePivotTable(rowdim="", rowlabels=[],
                           coldim = "", collabels=[],
                           cells=None)

        --arguments
          rowdim and coldim: The respective dimension labels as strings.
                             If empty, the dimension label is hidden.
          rowlabels: An optional list of CellText to label the rows.
          collabels: An optional list of CellText to label the columns.
          cells: A 1- or 2-dimensional sequence of values (CellText) for the table cells that
                 can be indexed as cells[i] or cells[i][j].

    --details:
          SimplePivotTable creates a pivot table with the given table values.
          It must be put into the block of "StartProcedure ... EndProcedure".
      For row and column labels, and cells, numbers will be converted to CellText.Number with default FormatSpec
      and strings in rowlabels will be converted to CellText.String.

          cells must match the label structure if labels are present.
          If there are no labels for the row dimension, the number of rows in Cells is used.
          If there are no labels for the column dimension, the length of the first cells item is used.
          In the absence of labels, if cells is one-dimensional, there will be one column (and many rows).

        --example
          import spss
          try:
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc1")
              table = spss.BasePivotTable("table of data","table of data","table")
              table.SimplePivotTable(rowdim = "rowdim",
                                     rowlabels=['rowa', 'rowb'],
                                     coldim = "coldim",
                                     collabels=['col1', 'coll2'],
                                     cells=[[1,2],[3,4]])
              spss.EndProcedure()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
    """

        error.Reset()

        try:
            # If cells is None, neither rowlabels nor collabels is allowed.
            if cells is None:
                if len(rowlabels) > 0:
                    error.SetErrorCode(1032)
                    if error.IsError():
                        raise SpssError(error)
                elif len(collabels) > 0:
                    error.SetErrorCode(1032)
                    if error.IsError():
                        raise SpssError(error)

            # Make a local copy. We don't want to change the original labels.
            tmpRowLabels = list(rowlabels)
            tmpColLabels = list(collabels)
        except TypeError:
            error.SetErrorCode(1004)
            if error.IsError():
                raise SpssError(error)

        # Check the structure of cells.
        nRows = 0
        nCols = 0

        # None or empty cells is okay at this point.
        if cells is not None:
            nRows = len(cells)
            if nRows > 0:
                if not isinstance(cells[0],str):
                    try:
                        #check if cells[0] is iterable.
                        nCols = len([(i, x) for (i,x) in enumerate(cells[0])])
                    except TypeError:
                        nCols = 1
                else:
                    nCols = 1

        if tmpRowLabels != [] and tmpColLabels != []:
            nRows = len(tmpRowLabels)
            nCols = len(tmpColLabels)
        elif tmpRowLabels != []:
            nRows = len(tmpRowLabels)
            # If there are no labels for the column dimension, the length of the first cells item is used.
            tmpColLabels.extend(["col"+str(x) for x in range(nCols)])
        elif tmpColLabels != []:
            nCols = len(tmpColLabels)
            # If there are no labels for the row dimension, the number of rows in Cells is used.
            tmpRowLabels.extend(["row"+str(x) for x in range(nRows)])
        else:
            tmpRowLabels.extend(["row"+str(x) for x in range(nRows)])
            tmpColLabels.extend(["col"+str(x) for x in range(nCols)])

        tmpRowLabels = list(map(CellText._CellText__ToCellText,tmpRowLabels))
        tmpColLabels = list(map(CellText._CellText__ToCellText,tmpColLabels))

        tmpCells = []

        # cells must match the label structure if the labels are given.
        if nRows > 0 and nCols > 0:
            try:
                # Check cells length and if cells can be indexed as cells[i][j] or cells[i].
                if nCols >= 1:
                    try:
                        x = []
                        islist = False
                        for c in cells:
                            if isinstance(c, (tuple,list)):
                                islist = True
                                x.append(len(c))
                            else:
                                x.append(1)
                        maxlen = max(x)
                    except TypeError:
                        maxlen = 1
                    if ( 1 == maxlen and not islist):
                        assert (len(cells) == nCols * nRows)
                        tmpCells = [cells[x*nCols + y] for x in range(nRows) for y in range(nCols)]
                    else:
                        assert(maxlen == nCols)
                        assert(len(cells) == nRows)
                        tmpCells = [cells[x][y] for x in range(nRows) for y in range(nCols)]
            except:
                error.SetErrorCode(1032)
                if error.IsError():
                    raise SpssError(error)

        # Check if cells[i][j] or cells[i] is scalar (such as sequence).
        for x in tmpCells:
            if not isinstance(x,(str, time.struct_time, datetime.datetime, datetime.date)):
                try:
                    [(i, x) for (i,x) in enumerate(x)]
                    error.SetErrorCode(1032)
                    if error.IsError():
                        raise SpssError(error)
                except TypeError:
                    pass

        tmpCells = list(map(CellText._CellText__ToCellText,tmpCells))

        # If dimension is empty, the dimension label is hidden.
        if rowdim == "":
            rowdim = self.Append(Dimension.Place.row,"rowdim",True,False)
        else:
            rowdim = self.Append(Dimension.Place.row,rowdim,False,False)
        if coldim == "":
            coldim = self.Append(Dimension.Place.column,"coldim",True,False)
        else:
            coldim = self.Append(Dimension.Place.column,coldim,False,False)

        if tmpCells != []:
            categories = [(row,col) for row in tmpRowLabels for col in tmpColLabels]
            for (i,cats) in enumerate(categories):
                self[cats] = tmpCells[i]


    def HideTitle(self):
        """
        Hides the title of the pivot table.

        --usage
          HideTitle()

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc")
              table = spss.BasePivotTable("table","mytable")
              table.Append(spss.Dimension.Place.row,"rowdim")
              table.Append(spss.Dimension.Place.column,"coldim")
              table[(spss.CellText.String("M"),spss.CellText.String("N"))] = spss.CellText.Number(23,spss.FormatSpec.Count)
              table.HideTitle()
              spss.EndProcedure()
              spss.StopSPSS()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
        """
        error.Reset()
        error.SetErrorCode(PyInvokeSpss.HidePivotTableTitle(spssutil.CheckStr(self.outline),
                                                            spssutil.CheckStr(self.title),
                                                            spssutil.CheckStr(self.templateName),
                                                            self.isSplit))
        if error.IsError():
            raise SpssError(error)
    def Insert(self,i,place,dimName,hideName=False, hideLabels=False):
        """
        Inserts a dimension to the pivot table.

        --usage
          Insert(i,place,dimName,hideName=False, hideLabels=False)

        --arguments
          i: The integer position of the dimension. Starts from 1. The lowest number is the inner dimension.
          place: The placement of the dimension which can be
                      Dimension.Place.row,
                      Dimension.Place.column, or
                      Dimension.Place.layer
          dimName: The string name of the dimension.
          hideName: Optional boolean that specifies whether the dimension name is hidden.
          hideLabels: Optional boolean that specifies whether the dimension labels are hidden.

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc")
              table = spss.BasePivotTable("table","mytable")

              table.Insert(1,spss.Dimension.Place.row,"rowdim-1")
              table.Insert(2,spss.Dimension.Place.row,"rowdim-2")
              table.Insert(1,spss.Dimension.Place.column,"coldim")

              table[(spss.CellText.String("M"),spss.CellText.String("W"),spss.CellText.String("N"))] = spss.CellText.Number(23,spss.FormatSpec.Count)
              spss.EndProcedure()
              spss.StopSPSS()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
        """
        error.Reset()
        pos = [x.position for x in self.dims if x.place == place]
        try:
            if i > max(pos)+1 or i < min(pos)-1:
                error.SetErrorCode(1031)
                if error.IsError():
                    raise SpssError(error)
        except ValueError:
            # pos may be empty.
            if i != 1:
                error.SetErrorCode(1031)
                if error.IsError():
                    raise SpssError(error)

        moves = [index for (index,dim) in enumerate(self.dims) if dim.place == place and dim.position >= i]
        for index in moves:
            self.dims[index].position += 1
            self.dims[index]._Dimension__update()

        dim = Dimension(dimName,place,i,hideName,hideLabels)
        dim._Dimension__append(self.outline,self.title,self.templateName,self.isSplit)
        if dim not in self.dims:
            self.dims.append(dim)

        return dim

    def Append(self,place,dimName,hideName=False, hideLabels=False):
        """
        Appends a dimension to the pivot table.

        --usage
          Append(place,dimName,hideName=False, hideLabels=False)

        --arguments
          place: The placement of the dimension which can be
                      Dimension.Place.row,
                      Dimension.Place.column, or
                      Dimension.Place.layer
          dimName: The string name of the dimension.
          hideName: Optional boolean that specifies whether the dimension name is hidden.
          hideLabels: Optional boolean that specifies whether the dimension labels are hidden.

        --example
            import spss
            try:
                spss.StartSPSS()
                spss.Submit("get file='demo.sav'.")
                spss.StartProcedure("proc")
                table = spss.BasePivotTable("table","mytable")
                table.Append(spss.Dimension.Place.row,"rowdim-1")
                table.Append(spss.Dimension.Place.row,"rowdim-2")
                table.Append(spss.Dimension.Place.column,"coldim")
                table[(spss.CellText.String("M"),spss.CellText.String("W"),spss.CellText.String("N"))] = spss.CellText.Number(23,spss.FormatSpec.Count)
                spss.EndProcedure()
                spss.StopSPSS()
            except spss.SpssError:
                print("Error.")
            print(spss.GetLastErrorMessage())
        """
        error.Reset()

        for (index, dim) in enumerate(self.dims):
            if dim.place == place:
                self.dims[index].position += 1
                self.dims[index]._Dimension__update()

        dim = Dimension(dimName,place,1,hideName,hideLabels)
        dim._Dimension__append(self.outline,self.title,self.templateName,self.isSplit)
        if dim not in self.dims:
            self.dims.append(dim)
        return dim

    def SetCategories(self,dim,categories):
        """
        Sets categories to a dimension.

        --usage
          SetCategories(dim,categories)

        --arguments
          dim: The dimension for which to set the categories.
          categories: Specifies the categories to set in the dimension.

    --details
          Dimensions must be added to the pivot table with either BasePivotTable.Append or BasePivotTable.Insert.
      Categories can be single category or a sequence of categories. Each category must be:
                  CellText.Number,
                  CellText.String,
                  CellText.VarName, or
                  CellText.VarValue.

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc")
              table = spss.BasePivotTable("table","mytable")
              row = table.Append(spss.Dimension.Place.row,"rowdim")
              column = table.Append(spss.Dimension.Place.column,"coldim")
              table.SetCategories(row,[spss.CellText.String("a"),spss.CellText.String("b")])
              table.SetCategories(column,[spss.CellText.String("c"),spss.CellText.String("d")])
              table.SetCellsByRow(spss.CellText.String("a"),(
                  spss.CellText.Number(23,spss.FormatSpec.Count),
                  spss.CellText.Number(24,spss.FormatSpec.Count))
                                  )
              table.SetCellsByRow(spss.CellText.String("b"),(
                  spss.CellText.Number(25,spss.FormatSpec.Count),
                  spss.CellText.Number(26,spss.FormatSpec.Count))
                                  )
              spss.EndProcedure()
              spss.StopSPSS()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
        """

        error.Reset()
        if not isinstance(dim, Dimension):
            error.SetErrorCode(1037)
            if error.IsError():
                raise SpssError(error)

        if dim not in self.dims:
            error.SetErrorCode()
            if error.IsError(1046):
                raise SpssError(error)

        if not isinstance(categories, (tuple,list)):
            dim._Dimension__SetCategory(categories)
        else:
            for cat in categories:
                dim._Dimension__SetCategory(cat)

    def SetCellsByRow(self,rowlabels,cells):
        """
        Sets cell values in the specified pivot table rows.

        --usage
          SetCellsByRow(rowlabels,cells)

        --arguments
          rowlabels: A tuple or list of the row categories for which cell values are specified.
          cells: A tuple or list of cell values that must match the structure of the column categories.

        --details
          The pivot table can have multiple row dimensions but only one column dimension.
          The categories must be set before calling this method.
      Tuple or list elements for row labels and cells must be
                  CellText.Number,
                  CellText.String,
                  CellText.VarName, or
                  CellText.VarValue

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc")
              table = spss.BasePivotTable("table","mytable")
              row = table.Append(spss.Dimension.Place.row,"rowdim")
              column = table.Append(spss.Dimension.Place.column,"coldim")

              table.SetCategories(row,spss.CellText.String("a"))
              table.SetCategories(row,spss.CellText.String("b"))
              table.SetCategories(column,spss.CellText.String("c"))
              table.SetCategories(column,spss.CellText.String("d"))

              table.SetCellsByRow(spss.CellText.String("a"),(
                  spss.CellText.Number(23,spss.FormatSpec.Count),
                  spss.CellText.Number(24,spss.FormatSpec.Count))
                                  )
              table.SetCellsByRow(spss.CellText.String("b"),(
                  spss.CellText.Number(25,spss.FormatSpec.Count),
                  spss.CellText.Number(26,spss.FormatSpec.Count))
                                  )
              spss.EndProcedure()
              spss.StopSPSS()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
        """
        error.Reset()

        if isinstance(rowlabels,list):
            rowlabels = tuple(rowlabels)
        elif not isinstance(rowlabels,tuple):
            rowlabels = (rowlabels,)

        rows = [x for x in self.dims if x.place == Dimension.Place.row]
        cols = [x for x in self.dims if x.place == Dimension.Place.column]
        if len(cols) > 1:
            error.SetErrorCode(1045)
            if error.IsError():
                raise SpssError(error)

        col = cols[0]

        if len(rows) != len(rowlabels):
            error.SetErrorCode(1038)
            raise SpssError(error)
        for i in range(len(rows)):
            rows[i]._Dimension__SetCategory(rowlabels[i])

        if len(col.categories) != len(cells):
            error.SetErrorCode(1032)
            raise SpssError(error)

        for (i,cat) in enumerate(col.categories):
            col._Dimension__SetCategory(cat)
            col._Dimension__SetCell(cells[i])
            self.cells[rowlabels + (cat,)] = cells[i]

    def SetCellsByColumn(self,collabels, cells):
        """
        Sets cell values in the speified pivot table columns.

        --usage
          SetCellsByColumn(collabels,cells)

        --arguments
          collabels: A tuple or list of column categories.
          cells: A tuple or list of cell values that must match the structure of the row categories.

        --details
          The pivot table can have multiple column dimensions but only one row dimension.
          The categories must be set before calling this method.
          Tuple or list elements for column labels and cells must be
                  CellText.Number,
                  CellText.String,
                  CellText.VarName, or
                  CellText.VarValue

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc")
              table = spss.BasePivotTable("table","mytable")
              row = table.Append(spss.Dimension.Place.row,"rowdim")
              column = table.Append(spss.Dimension.Place.column,"coldim")

              table.SetCategories(row,spss.CellText.String("a"))
              table.SetCategories(row,spss.CellText.String("b"))
              table.SetCategories(column,spss.CellText.String("c"))
              table.SetCategories(column,spss.CellText.String("d"))

              table.SetCellsByColumn(spss.CellText.String("c"),(
                  spss.CellText.Number(23,spss.FormatSpec.Count),
                  spss.CellText.Number(24,spss.FormatSpec.Count))
                                  )
              table.SetCellsByColumn(spss.CellText.String("d"),(
                  spss.CellText.Number(25,spss.FormatSpec.Count),
                  spss.CellText.Number(26,spss.FormatSpec.Count))
                                  )
              spss.EndProcedure()
              spss.StopSPSS()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
        """
        error.Reset()

        if isinstance(collabels,list):
            collabels = tuple(collabels)
        elif not isinstance(collabels,tuple):
            collabels = (collabels,)

        cols = [x for x in self.dims if x.place == Dimension.Place.column]
        rows = [x for x in self.dims if x.place == Dimension.Place.row]
        if len(rows) > 1:
            error.SetErrorCode(1044)
            if error.IsError():
                raise SpssError(error)

        row = rows[0]

        if len(cols) != len(collabels):
            error.SetErrorCode(1038)
            raise SpssError(error)
        for i in range(len(cols)):
            cols[i]._Dimension__SetCategory(collabels[i])

        if len(row.categories) != len(cells):
            error.SetErrorCode(1032)
            raise SpssError(error)

        for (i,cat) in enumerate(row.categories):
            row._Dimension__SetCategory(cat)
            row._Dimension__SetCell(cells[i])
            self.cells[collabels + (cat,)] = cells[i]

    def SetCell(self,cell):
        """
        Sets the cell value of the current category of the pivot table.

        --usage
          SetCell(cell)

        --arguments
          cell: The cell values which must be one of
                      CellText.Number,
                      CellText.String,
                      CellText.VarName, or
                      CellText.VarValue.

        --details
          The categories must be set before calling this method.

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.Submit("get file='demo.sav'.")
              spss.StartProcedure("proc")
              table = spss.BasePivotTable("table","mytable")

              row = table.Append(spss.Dimension.Place.row,"rowdim")
              column = table.Append(spss.Dimension.Place.column,"coldim")

              table.SetCategories(row,spss.CellText.String("a"))
              table.SetCategories(column,spss.CellText.String("c"))
              table.SetCell(spss.CellText.Number(23,spss.FormatSpec.Count))
              table.SetCategories(column,spss.CellText.String("d"))
              table.SetCell(spss.CellText.Number(24,spss.FormatSpec.Count))

              table.SetCategories(row,spss.CellText.String("b"))
              table.SetCategories(column,spss.CellText.String("c"))
              table.SetCell(spss.CellText.Number(25,spss.FormatSpec.Count))
              table.SetCategories(column,spss.CellText.String("d"))
              table.SetCell(spss.CellText.Number(26,spss.FormatSpec.Count))

              spss.EndProcedure()
              spss.StopSPSS()
          except spss.SpssError:
              print("Error.")
          print(spss.GetLastErrorMessage())
        """
        error.Reset()

        dim = self.dims[-1]
        dim._Dimension__SetCategory(dim.current)
        dim._Dimension__SetCell(cell)

        categories = (dim.current for dim in self.dims)
        self.cells[categories] = cell

    def SetDefaultFormatSpec(self,formatSpec,varIndex=None):
        """
        Set the default format for the pivot table.

        --usage
          SetDefaultFormatSpec(format)

        --arguments
          formatSpec: An integer. Valid settings include:
                        Coefficient     =0
                        CoefficientSE   =1
                        CoefficientVar  =2
                        Correlation     =3
                        GeneralStat     =4
                        Mean            =5
                        Count           =6
                        Percent         =7
                        PercentNoSign   =8
                        Proportion      =9
                        Significance    =10
                        Residual        =11
                        Variable        =12
                        StdDev          =13
                        Difference      =14
                        Sum             =15
          varIndex: The index of a variable in the active dataset whose format is used to determine details of
                    the specified format. The varIndex argument is only used for the following formats:
                        Mean            =5
                        Variable        =12
                        StdDev          =13
                        Difference      =14
                        Sum             =15

        --example
            import spss
            try:
                spss.StartSPSS()
                spss.Submit("get file='demo.sav'.")
                spss.StartProcedure("proc")
                table = spss.BasePivotTable("table","mytable")

                table.SetDefaultFormatSpec(spss.FormatSpec.Mean,0)

                table.Append(spss.Dimension.Place.row,"rowdim")
                table.Append(spss.Dimension.Place.column,"coldim")
                table[(spss.CellText.String("M"),spss.CellText.String("N"))] = spss.CellText.Number(23)
                table.Caption("My Caption.")
                spss.EndProcedure()
                spss.StopSPSS()
            except spss.SpssError:
                print("Error.")
            print(spss.GetLastErrorMessage())

        """
        error.Reset()
        CellText._CellText__SetDefaultFormatSpec([formatSpec,varIndex])

    def GetDefaultFormatSpec(self):
        """
        Get the default formatSpec.

        --usage
          GetDefaultFormatSpec()

        --value
          Returns a list.

        --details
          The first element in the returned list is the FormatSpec:
                        Coefficient     =0
                        CoefficientSE   =1
                        CoefficientVar  =2
                        Correlation     =3
                        GeneralStat     =4
                        Mean            =5
                        Count           =6
                        Percent         =7
                        PercentNoSign   =8
                        Proportion      =9
                        Significance    =10
                        Residual        =11
                        Variable        =12
                        StdDev          =13
                        Difference      =14
                        Sum             =15
          The second element in the returned list is a variable index used for the FormatSpec, if any.

        --example
            import spss
            try:
                spss.StartSPSS()
                spss.Submit("get file='demo.sav'.")
                spss.StartProcedure("proc")
                table = spss.BasePivotTable("table","mytable")

                format = table.GetDefaultFormatSpec()
                print("The default formatSpec = ", format)

                table.Append(spss.Dimension.Place.row,"rowdim")
                table.Append(spss.Dimension.Place.column,"coldim")

                table.SetDefaultFormatSpec(spss.FormatSpec.Percent)
                print("Change the default formatSpec to spss.FormatSpec.Percent")

                table[(spss.CellText.String("M"),spss.CellText.String("N"))] = spss.CellText.Number(23)
                table.Caption("My Caption.")
                spss.EndProcedure()
                spss.StopSPSS()
            except spss.SpssError:
                print("Error.")
            print(spss.GetLastErrorMessage())

        """
        error.Reset()
        return CellText._CellText__GetDefaultFormatSpec()

class _const:
    """
    Defines a constants class. It will be used to define constants.
    """
    class __impl:
        class ConstError(TypeError): pass
        def __setattr__(self, name, value):
            if name in self.__dict__:
                raise self.ConstError("Can't rebind const(%s)"%name)
            self.__dict__[name] = value
        def __delattr__(self, name):
            if name in self.__dict__:
                raise self.ConstError("Can't unbind const(%s)"%name)
            raise NameError(name)

    __instance = __impl()
    def __getattr__(self,attr):
        return getattr(self.__instance, attr)
    def __setattr__(self, attr, value):
        return setattr(self.__instance, attr, value)


class Dimension(object):
    """
    Dimension class. Don't use it directly except for its constants below:
        Place.row
        Place.column
        Place.layer
    """

    # Defines Place constants.
    Place = _const()
    Place.row = 0
    Place.column = 1
    Place.layer = 2

    def __CheckPlace(place):
        """
        Checks if placement is an invalid dimension. If so, an exception will be raised.
        """
        if place not in [Dimension.Place.row, Dimension.Place.column, Dimension.Place.layer]:
            error.SetErrorCode(1042)
            if error.IsError():
                raise SpssError(error)
    __CheckPlace = staticmethod(__CheckPlace)


    def __init__(self,name,place,position,hideName=False,hideLabels=False):
        """
        Creates an instance of Dimension. Can be created and returned only by
        BasePivotTable.Append, or BasePivotTable.Insert.
        """

        error.Reset()
        self.__CheckPlace(place)

        if not isinstance(position,int):
            error.SetErrorCode(1000)
            if error.IsError():
                raise SpssError(error)

        if not isinstance(hideName,bool):
            error.SetErrorCode(1008)
            if error.IsError():
                raise SpssError(error)

        if not isinstance(hideLabels,bool):
            error.SetErrorCode(1008)
            if error.IsError():
                raise SpssError(error)

        self.name = name
        self.place = place
        self.position = position
        self.hideName = hideName
        self.hideLabels = hideLabels
        self.tableAttr = {}
        self.categories = []
        self.current = None

    def __append(self,outline,title,templateName,isSplit=False):
        """
        Appends the dimension to a pivot table.
        Can be called by BasePivotTable.Append or BasePivotTable.Insert only.
        """
        error.Reset()
        
        #Checks the template name. It should start with a letter and has the max length 64 bytes.
        if len(templateName) < 1 or len(templateName) > 64:
            error.SetErrorCode(1036)
            if error.IsError():
                raise SpssError(error)
        if not templateName[0].isalpha():
            error.SetErrorCode(1036)
            if error.IsError():
                raise SpssError(error)

        if not isinstance(isSplit,bool):
            error.SetErrorCode(1008)
            if error.IsError():
                raise SpssError(error)

        self.tableAttr["outline"] = outline
        self.tableAttr["title"] = title
        self.tableAttr["templateName"] = templateName
        self.tableAttr["isSplit"] = isSplit

        error.SetErrorCode(PyInvokeSpss.AddDimension(spssutil.CheckStr(self.tableAttr["outline"]),
                                                     spssutil.CheckStr(self.tableAttr["title"]),
                                                     spssutil.CheckStr(self.tableAttr["templateName"]),
                                                     self.tableAttr["isSplit"],
                                                     spssutil.CheckStr(self.name),
                                                     self.place,
                                                     self.position,
                                                     self.hideName,
                                                     self.hideLabels))
        if error.IsError():
            raise SpssError(error)


    def __update(self):
        """
        Updates the dimension properties.
        Can be called by BasePivotTable.Append or BasePivotTable.Insert only.
        """
        error.Reset()
        error.SetErrorCode(PyInvokeSpss.AddDimension(spssutil.CheckStr(self.tableAttr["outline"]),
                                                     spssutil.CheckStr(self.tableAttr["title"]),
                                                     spssutil.CheckStr(self.tableAttr["templateName"]),
                                                     self.tableAttr["isSplit"],
                                                     spssutil.CheckStr(self.name),
                                                     self.place,
                                                     self.position,
                                                     self.hideName,
                                                     self.hideLabels))
        if error.IsError():
            raise SpssError(error)


    def __SetCategory(self,category):
        """
        Sets category to the dimension.
        """

        error.Reset()
        CellText._CellText__CheckType(category)

        if category not in self.categories:
            self.categories.append(category)
        self.current = category

        if 0 == category.data["type"]: #Number
            format = category.data["format"]
            CellText._CellText__SetFormatSpec(format)

            error.SetErrorCode(PyInvokeSpss.AddNumberCategory(spssutil.CheckStr(self.tableAttr["outline"]),
                                                              spssutil.CheckStr(self.tableAttr["title"]),
                                                              spssutil.CheckStr(self.tableAttr["templateName"]),
                                                              self.tableAttr["isSplit"],
                                                              spssutil.CheckStr(self.name),
                                                              self.place,
                                                              self.position,
                                                              self.hideName,
                                                              self.hideLabels,
                                                              category.data["value"]))
            if error.IsError():
                raise SpssError(error)

        elif 1 == category.data["type"]: #String
            error.SetErrorCode(PyInvokeSpss.AddStringCategory(spssutil.CheckStr(self.tableAttr["outline"]),
                                                              spssutil.CheckStr(self.tableAttr["title"]),
                                                              spssutil.CheckStr(self.tableAttr["templateName"]),
                                                              self.tableAttr["isSplit"],
                                                              spssutil.CheckStr(self.name),
                                                              self.place,
                                                              self.position,
                                                              self.hideName,
                                                              self.hideLabels,
                                                              spssutil.CheckStr(category.data["value"])))
            if error.IsError():
                raise SpssError(error)
        elif 2 == category.data["type"]: #varName
            error.SetErrorCode(PyInvokeSpss.AddVarNameCategory(spssutil.CheckStr(self.tableAttr["outline"]),
                                                               spssutil.CheckStr(self.tableAttr["title"]),
                                                               spssutil.CheckStr(self.tableAttr["templateName"]),
                                                               self.tableAttr["isSplit"],
                                                               spssutil.CheckStr(self.name),
                                                               self.place,
                                                               self.position,
                                                               self.hideName,
                                                               self.hideLabels,
                                                               category.data["varID"]))
            if error.IsError():
                raise SpssError(error)
        elif 3 == category.data["type"]: #varValue
            if isinstance(category.data["value"],str):
                error.SetErrorCode(PyInvokeSpss.AddVarValueStringCategory(spssutil.CheckStr(self.tableAttr["outline"]),
                                                                          spssutil.CheckStr(self.tableAttr["title"]),
                                                                          spssutil.CheckStr(self.tableAttr["templateName"]),
                                                                          self.tableAttr["isSplit"],
                                                                          spssutil.CheckStr(self.name),
                                                                          self.place,
                                                                          self.position,
                                                                          self.hideName,
                                                                          self.hideLabels,
                                                                          category.data["varID"],
                                                                          category.data["value"]))
                if error.IsError():
                    raise SpssError(error)
            else:
                error.SetErrorCode(PyInvokeSpss.AddVarValueDoubleCategory(spssutil.CheckStr(self.tableAttr["outline"]),
                                                                          spssutil.CheckStr(self.tableAttr["title"]),
                                                                          spssutil.CheckStr(self.tableAttr["templateName"]),
                                                                          self.tableAttr["isSplit"],
                                                                          spssutil.CheckStr(self.name),
                                                                          self.place,
                                                                          self.position,
                                                                          self.hideName,
                                                                          self.hideLabels,
                                                                          category.data["varID"],
                                                                          category.data["value"]))
                if error.IsError():
                    raise SpssError(error)

    def __SetCell(self,cell):
        """
        Sets cell in the current category.
        """
        error.Reset()
        CellText._CellText__CheckType(cell)

        if 0 == cell.data["type"]: #Number
            format = cell.data["format"]
            CellText._CellText__SetFormatSpec(format)

            error.SetErrorCode(PyInvokeSpss.SetNumberCell(spssutil.CheckStr(self.tableAttr["outline"]),
                                                          spssutil.CheckStr(self.tableAttr["title"]),
                                                          spssutil.CheckStr(self.tableAttr["templateName"]),
                                                          self.tableAttr["isSplit"],
                                                          spssutil.CheckStr(self.name),
                                                          self.place,
                                                          self.position,
                                                          self.hideName,
                                                          self.hideLabels,
                                                          cell.data["value"]))
            if error.IsError():
                raise SpssError(error)

        elif 1 == cell.data["type"]: #String
            error.SetErrorCode(PyInvokeSpss.SetStringCell(spssutil.CheckStr(self.tableAttr["outline"]),
                                                          spssutil.CheckStr(self.tableAttr["title"]),
                                                          spssutil.CheckStr(self.tableAttr["templateName"]),
                                                          self.tableAttr["isSplit"],
                                                          spssutil.CheckStr(self.name),
                                                          self.place,
                                                          self.position,
                                                          self.hideName,
                                                          self.hideLabels,
                                                          spssutil.CheckStr(cell.data["value"])))
            if error.IsError():
                raise SpssError(error)

        elif 2 == cell.data["type"]: #varName
            error.SetErrorCode(PyInvokeSpss.SetVarNameCell(spssutil.CheckStr(self.tableAttr["outline"]),
                                                           spssutil.CheckStr(self.tableAttr["title"]),
                                                           spssutil.CheckStr(self.tableAttr["templateName"]),
                                                           self.tableAttr["isSplit"],
                                                           spssutil.CheckStr(self.name),
                                                           self.place,
                                                           self.position,
                                                           self.hideName,
                                                           self.hideLabels,
                                                           cell.data["varID"]))
            if error.IsError():
                raise SpssError(error)

        elif 3 == cell.data["type"]: #varValue
            if isinstance(cell.data["value"],str):
                error.SetErrorCode(PyInvokeSpss.SetVarValueStringCell(spssutil.CheckStr(self.tableAttr["outline"]),
                                                                      spssutil.CheckStr(self.tableAttr["title"]),
                                                                      spssutil.CheckStr(self.tableAttr["templateName"]),
                                                                      self.tableAttr["isSplit"],
                                                                      spssutil.CheckStr(self.name),
                                                                      self.place,
                                                                      self.position,
                                                                      self.hideName,
                                                                      self.hideLabels,
                                                                      cell.data["varID"],
                                                                      spssutil.CheckStr(cell.data["value"])))
                if error.IsError():
                    raise SpssError(error)
            else:
                error.SetErrorCode(PyInvokeSpss.SetVarValueDoubleCell(spssutil.CheckStr(self.tableAttr["outline"]),
                                                                      spssutil.CheckStr(self.tableAttr["title"]),
                                                                      spssutil.CheckStr(self.tableAttr["templateName"]),
                                                                      self.tableAttr["isSplit"],
                                                                      spssutil.CheckStr(self.name),
                                                                      self.place,
                                                                      self.position,
                                                                      self.hideName,
                                                                      self.hideLabels,
                                                                      cell.data["varID"],
                                                                      cell.data["value"]))
                if error.IsError():
                    raise SpssError(error)


    def __AddFootnotes(self, footnotes):
        """
        Adds footnotes to the current cell.
        """
        error.Reset()
        error.SetErrorCode(PyInvokeSpss.AddCellFootnotes(spssutil.CheckStr(self.tableAttr["outline"]),
                                                         spssutil.CheckStr(self.tableAttr["title"]),
                                                         spssutil.CheckStr(self.tableAttr["templateName"]),
                                                         self.tableAttr["isSplit"],
                                                         spssutil.CheckStr(self.name),
                                                         self.place,
                                                         self.position,
                                                         self.hideName,
                                                         self.hideLabels,
                                                         spssutil.CheckStr(footnotes)))
        if error.IsError():
            raise SpssError(error)

    def __AddDimFootnotes(self, footnotes):
        """
        Adds footnotes to the current cell.
        """
        error.Reset()
        error.SetErrorCode(PyInvokeSpss.AddDimFootnotes( spssutil.CheckStr(self.tableAttr["outline"]),
                                                         spssutil.CheckStr(self.tableAttr["title"]),
                                                         spssutil.CheckStr(self.tableAttr["templateName"]),
                                                         self.tableAttr["isSplit"],
                                                         spssutil.CheckStr(self.name),
                                                         self.place,
                                                         self.position,
                                                         self.hideName,
                                                         self.hideLabels,
                                                         spssutil.CheckStr(footnotes)))
        if error.IsError():
            raise SpssError(error)

    def __AddCategoryFootnotes(self, footnotes):
        """
        Adds footnotes to the current cell.
        """
        error.Reset()
        error.SetErrorCode(PyInvokeSpss.AddCategoryFootnotes(spssutil.CheckStr(self.tableAttr["outline"]),
                                                             spssutil.CheckStr(self.tableAttr["title"]),
                                                             spssutil.CheckStr(self.tableAttr["templateName"]),
                                                             self.tableAttr["isSplit"],
                                                             spssutil.CheckStr(self.name),
                                                             self.place,
                                                             self.position,
                                                             self.hideName,
                                                             self.hideLabels,
                                                             spssutil.CheckStr(footnotes)))
        if error.IsError():
            raise SpssError(error)


    def __len__(self): return len(self.categories)

    def __contains__(self,category): return category in self.categories

    def __iter__(self):
        for category in self.categories:
            yield category

    def __getitem__(self,i): return self.categories[i]

    def __setitem__(self, i, category):
        """
        Disable __setitem__.
        """
        error.SetErrorCode(1048)
        if error.IsError():
            raise SpssError(error)

    def __delitem__(self, i):
        """
        Disable __delitem__.
        """
        error.SetErrorCode(1047)
        if error.IsError():
            raise SpssError(error)

    def __repr__(self):
        if 0 == self.place:
            tellPlace = "row dimension"
        elif 1 == self.place:
            tellPlace = "column dimension"
        return repr([self.name, tellPlace])


class CellText(object):
    """
    CellText class. It includes:
        CellText.Number,
        CellText.String,
        CellText.VarName,
        CellText.VarValue
    Which can be used to create a category or a cell in a pivot table.
    """

    __formatDict = {
        FormatSpec.Coefficient:    PyInvokeSpss.SetFormatSpecCoefficient,
        FormatSpec.CoefficientSE:  PyInvokeSpss.SetFormatSpecCoefficientSE,
        FormatSpec.CoefficientVar: PyInvokeSpss.SetFormatSpecCoefficientVar,
        FormatSpec.Correlation:    PyInvokeSpss.SetFormatSpecCorrelation,
        FormatSpec.GeneralStat:    PyInvokeSpss.SetFormatSpecGeneralStat,
        FormatSpec.Mean:           PyInvokeSpss.SetFormatSpecMean,
        FormatSpec.Count:          PyInvokeSpss.SetFormatSpecCount,
        FormatSpec.Percent:        PyInvokeSpss.SetFormatSpecPercent,
        FormatSpec.PercentNoSign:  PyInvokeSpss.SetFormatSpecPercentNoSign,
        FormatSpec.Proportion:     PyInvokeSpss.SetFormatSpecProportion,
        FormatSpec.Significance:   PyInvokeSpss.SetFormatSpecSignificance,
        FormatSpec.Residual:       PyInvokeSpss.SetFormatSpecResidual,
        FormatSpec.Variable:       PyInvokeSpss.SetFormatSpecVariable,
        FormatSpec.StdDev:         PyInvokeSpss.SetFormatSpecStdDev,
        FormatSpec.Difference:     PyInvokeSpss.SetFormatSpecDifference,
        FormatSpec.Sum:            PyInvokeSpss.SetFormatSpecSum
        }

    __defaultFormatSpec = [FormatSpec.GeneralStat]

    def __CheckType(aCellText):
        error.Reset()
        if not isinstance(aCellText,(CellText.Number,CellText.String,CellText.VarName,CellText.VarValue)):
            error.SetErrorCode(1039)
            if error.IsError():
                raise SpssError(error)

    def __CheckFormatSpec(format):
        error.Reset()
        try:
            formatSpec = format[0]
        except :
            error.SetErrorCode(1049)
            if error.IsError():
                raise SpssError(error)

        try:
            varIndex = format[1]
        except :
            varIndex = None

        if not isinstance(formatSpec,int) or formatSpec < 0 or formatSpec > 15:
            error.SetErrorCode(1049)
            if error.IsError():
                raise SpssError(error)

        # Some formatSpec require a varIndex argument.
        if formatSpec in [FormatSpec.Mean,
                          FormatSpec.Variable,
                          FormatSpec.StdDev,
                          FormatSpec.Difference,
                          FormatSpec.Sum]:
            if varIndex is None:
                error.SetErrorCode(1050)
                if error.IsError():
                    raise SpssError(error)

    def __SetFormatSpec(format):
        error.Reset()
        CellText._CellText__CheckFormatSpec(format)

        if format[0] in [FormatSpec.Mean,
                         FormatSpec.Variable,
                         FormatSpec.StdDev,
                         FormatSpec.Difference,
                         FormatSpec.Sum]:
            # Some formatSpec require a varIndex argument.
            CellText._CellText__formatDict[format[0]](format[1])
        else:
            # The others have not the argument requirement.
            CellText._CellText__formatDict[format[0]]()

    def __SetDefaultFormatSpec(format):
        error.Reset()
        CellText._CellText__CheckFormatSpec(format)
        CellText._CellText__defaultFormatSpec = format

    def __GetDefaultFormatSpec():
        error.Reset()
        return CellText._CellText__defaultFormatSpec

    def __ToCellText(obj):
        error.Reset()
        if obj is None:
            obj = PyInvokeSpss.GetSystemMissingValue( )
        if isinstance(obj,(CellText.Number,CellText.String,CellText.VarName,CellText.VarValue)): return obj
        elif isinstance(obj, str): return CellText.String(obj)
        elif isinstance(obj, (int,float,time.struct_time,datetime.date)): return CellText.Number(obj)
        else:
            error.SetErrorCode(1004)
            if error.IsError():
                raise SpssError(error)

    __CheckType = staticmethod(__CheckType)
    __CheckFormatSpec = staticmethod(__CheckFormatSpec)

    __SetFormatSpec = staticmethod(__SetFormatSpec)
    __SetDefaultFormatSpec = staticmethod(__SetDefaultFormatSpec)
    __GetDefaultFormatSpec = staticmethod(__GetDefaultFormatSpec)
    __ToCellText = staticmethod(__ToCellText)

    class Number(object):
        """
        Creates a CellText.Number object.

        --usage
          aCellText = CellText.Number(value,formatSpec,varIndex=None)

        --arguments
          value: The numeric value of the CellText object.
          formatSpec: An integer. Valid settings include:
                        Coefficient     =0
                        CoefficientSE   =1
                        CoefficientVar  =2
                        Correlation     =3
                        GeneralStat     =4
                        Mean            =5
                        Count           =6
                        Percent         =7
                        PercentNoSign   =8
                        Proportion      =9
                        Significance    =10
                        Residual        =11
                        Variable        =12
                        StdDev          =13
                        Difference      =14
                        Sum             =15
          varIndex: The index of a variable in the active dataset whose format is used to determine
                    details of the specified format. The varIndex argument is only used for the following formats:
                        Mean            =5
                        Variable        =12
                        StdDev          =13
                        Difference      =14
                        Sum             =15

        --example
          cell = CellText.Number(23, FormatSpec.Mean, varIndex=0)

        """
        def __init__(self,value,formatSpec=None,varIndex=None):
            error.Reset()
            if isinstance(value, time.struct_time):
                value = datetime.datetime(value.tm_year, value.tm_mon, value.tm_mday, value.tm_hour, value.tm_min, value.tm_sec)
            if isinstance(value, datetime.date):
                if not isinstance(value, datetime.datetime):
                    value = datetime.datetime(value.year, value.month, value.day, 0, 0, 0)
            if isinstance(value, datetime.datetime):
                origtime = datetime.datetime(1582, 10, 14, 0, 0, 0)
                diffdays = (value-origtime).days
                newtime = datetime.datetime(value.year, value.month, value.day, 0, 0, 0)
                diffsecs = (value-newtime).seconds
                value = diffdays*86400 + diffsecs
            if isinstance(value, str):
                try:
                    value = float(value)
                except :
                    error.SetErrorCode(1004)
                    if error.IsError():
                        raise SpssError(error)
            else:
                if value is None:
                    value = PyInvokeSpss.GetSystemMissingValue( )
                if not isinstance(value,(int,float)):                
                    error.SetErrorCode(1004)
                    if error.IsError():
                        raise SpssError(error)
            if not (isinstance(varIndex, int) or varIndex is None):
                error.SetErrorCode(1004)
                if error.IsError():
                    raise SpssError(error)

            self.data = {}
            self.data["type"] = 0
            self.data["value"] = float(value)
            if formatSpec is None:
                self.data["format"] = CellText._CellText__defaultFormatSpec
            else:
                self.data["format"] = (formatSpec,varIndex)

        def __eq__(self,obj):
            CellText._CellText__CheckType(obj)
            return operator.eq(self.data,obj.data)
        
        def __ne__(self,obj):
            CellText._CellText__CheckType(obj)
            return operator.ne(self.data,obj.data)
        
        def __hash__(self):
            return id(self.data)

        def toNumber(self): return self.data["value"]
        def toString(self): return str(self.data["value"])

        def __del__(self): self.data = {}
        def __repr__(self): return repr(self.data)


    class String(object):
        """
        Creates a CellText.String object.

        --usage
          aCellText = CellText.String(value)

        --arguments
          value: The string value of the CellText object.

        --example
          cell = CellText.String("a")

        """

        def __init__(self,value):
            error.Reset()
            self.data = {}
            self.data["type"] = 1
            if isinstance(value, (int,float)):
                self.data["value"] = str(value)
            else:
                self.data["value"] = value

        def __eq__(self,obj):
            CellText._CellText__CheckType(obj)
            return operator.eq(self.data,obj.data)
        
        def __ne__(self,obj):
            CellText._CellText__CheckType(obj)
            return operator.ne(self.data,obj.data)
        
        def __hash__(self):
            return id(self.data)

        def toNumber(self):
            try:
                return float(self.data["value"])
            except ValueError:
                return None

        def toString(self): return self.data["value"]

        def __del__(self): self.data = {}
        def __repr__(self): return repr(self.data)


    class VarName(object):
        """
        Creates a CellText.VarName object.

        --usage
          aCellText = CellText.VarName(index)

        --arguments
          index: The index of the variable.

        --example
          cell = CellText.VarName(0)

        """

        def __init__(self,index):
            error.Reset()
            if not isinstance(index, int):
                error.SetErrorCode(1004)
                if error.IsError():
                    raise SpssError(error)

            self.data = {}
            self.data["type"] = 2
            self.data["varID"] = index

        def __eq__(self,obj):
            CellText._CellText__CheckType(obj)
            return operator.eq(self.data,obj.data)
        
        def __ne__(self,obj):
            CellText._CellText__CheckType(obj)
            return operator.ne(self.data,obj.data)
        
        def __hash__(self):
            return id(self.data)

        def toNumber(self): return None
        def toString(self): return None
        def __del__(self): self.data = {}
        def __repr__(self): return repr(self.data)


    class VarValue(object):
        """
        Creates a CellText.VarValue object.

        --usage
          aCellText = CellText.VarValue(index,value)

        --arguments
          index: The index of the variable.
          value: The number or string value of the CellText object.

        --example
          cell = CellText.VarValue(1,"f")

        """

        def __init__(self,index,value):
            error.Reset()
            if not isinstance(index, int):
                error.SetErrorCode(1004)
                if error.IsError():
                    raise SpssError(error)

            self.data = {}
            self.data["type"] = 3
            self.data["varID"] = index

            varType = spss.GetVariableType(index)
            if varType > 0:
                value = str(value)

            if isinstance(value,str):
                self.data["value"] = value
            elif isinstance(value,(int,float)):
                self.data["value"] = float(value)
            else:
                error.SetErrorCode(1051)
                if error.IsError():
                    raise SpssError(error)

        def __eq__(self,obj):
            CellText._CellText__CheckType(obj)
            return operator.eq(self.data,obj.data)
        
        def __ne__(self,obj):
            CellText._CellText__CheckType(obj)
            return operator.ne(self.data,obj.data)
        
        def __hash__(self):
            return id(self.data)

        def toNumber(self): return None
        def toString(self): return None
        def __del__(self): self.data = {}
        def __repr__(self): return repr(self.data)


class TextBlock(object):
    """
    TextBlock class used to create or append a text block to a outline.
    """
    def __init__(self,name,content,outline=""):
        """
        Creates a TextBlock object.

        --usage
          text = TextBlock(outline,name,content)

        --arguments
          name: The string name of the TextBlock.
          content: The string content of the TextBlock.
          outline: The optional string outline title for the TextBlock.
                   If the outline title is not specified, the TextBlock will be placed one level under
                   the root item for the procedure that created it. Otherwise, the TextBlock will be
                   placed under the specified outline title.
        --example
            import spss
            try:
                spss.Submit("get file='demo.sav'.")
                spss.StartProcedure("proc")
                text = spss.TextBlock("text","hello")
            finally:
                spss.EndProcedure()
                spss.StopSPSS()

        """

        error.Reset()
        self.__outline = outline
        self.__name = name
        self.__content = content
        self.__skip = 1

        error.SetErrorCode(PyInvokeSpss.AddTextBlock(spssutil.CheckStr(self.__outline),
                                                     spssutil.CheckStr(self.__name),
                                                     spssutil.CheckStr(self.__content),
                                                     self.__skip))
        if error.IsError():
            raise SpssError(error)

    def append(self,line,skip=1):
        """
        Appends a line to an existing TextBlock object.

        --usage
          append(line,skip=1)

        --arguments
          line: The string content to append to the TextBlock.
          skip: The number of new lines to skip.

        --example
            import spss
            try:
                spss.Submit("get file='demo.sav'.")
                spss.StartProcedure("proc")
                text = spss.TextBlock("text","hello")
                text.append("world",skip=1)
            finally:
                spss.EndProcedure()
                spss.StopSPSS()

        """
        error.Reset()
        if not isinstance(skip,int):
            error.SetErrorCode(1000)
            if error.IsError():
                raise SpssError(error)

        self.__content += "\n"
        self.__content += line
        self.__skip = skip

        error.SetErrorCode(PyInvokeSpss.AddTextBlock(spssutil.CheckStr(self.__outline),
                                                     spssutil.CheckStr(self.__name),
                                                     spssutil.CheckStr(line),
                                                     self.__skip))
        if error.IsError():
            raise SpssError(error)

    def __del__(self): self.__content = ""
    def __repr__(self): return repr(self.__content.split('\n'))


__all__ = ["BasePivotTable","Dimension","CellText","TextBlock"]

from . import version
__version__ = version.version

