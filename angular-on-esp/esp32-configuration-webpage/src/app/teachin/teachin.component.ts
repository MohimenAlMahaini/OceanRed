import { Component, OnInit } from '@angular/core';
import { MatDialog } from '@angular/material/dialog'
import { TeachInDialogComponent } from '../teach-in-dialog/teach-in-dialog.component';

@Component({
  selector: 'app-teachin',
  templateUrl: './teachin.component.html',
  styleUrls: ['./teachin.component.css']
})
export class TeachinComponent implements OnInit {

  constructor(public dialog: MatDialog) { }

  ngOnInit(): void {
  }

  openDialog() {
    this.dialog.open(TeachInDialogComponent);
  }
}

