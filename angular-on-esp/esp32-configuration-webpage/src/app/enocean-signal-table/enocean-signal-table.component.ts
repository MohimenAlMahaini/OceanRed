import { AfterViewInit, Component, ViewChild } from '@angular/core';
import { MatPaginator } from '@angular/material/paginator';
import { MatSort } from '@angular/material/sort';
import { MatTable } from '@angular/material/table';
import { EnoceanSignalTableDataSource, EnoceanSignalTableItem } from './enocean-signal-table-datasource';

@Component({
  selector: 'app-enocean-signal-table',
  templateUrl: './enocean-signal-table.component.html',
  styleUrls: ['./enocean-signal-table.component.css']
})
export class EnoceanSignalTableComponent implements AfterViewInit {
  @ViewChild(MatPaginator) paginator!: MatPaginator;
  @ViewChild(MatSort) sort!: MatSort;
  @ViewChild(MatTable) table!: MatTable<EnoceanSignalTableItem>;
  dataSource: EnoceanSignalTableDataSource;

  /** Columns displayed in the table. Columns IDs can be added, removed, or reordered. */
  displayedColumns = ['id', 'name'];

  constructor() {
    this.dataSource = new EnoceanSignalTableDataSource();
  }

  ngAfterViewInit(): void {
    this.dataSource.sort = this.sort;
    this.dataSource.paginator = this.paginator;
    this.table.dataSource = this.dataSource;
  }
}
