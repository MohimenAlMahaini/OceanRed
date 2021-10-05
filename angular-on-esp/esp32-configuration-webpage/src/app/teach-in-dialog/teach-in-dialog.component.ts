import { Component, OnInit } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';

@Component({
  selector: 'app-teach-in-dialog',
  templateUrl: './teach-in-dialog.component.html',
  styleUrls: ['./teach-in-dialog.component.css']
})
export class TeachInDialogComponent implements OnInit {

  inputString: string = "";
  stage = 0;

  constructor(public dialog: MatDialog) { }

  ngOnInit(): void {
  }

  openEnOceanDialog(functionname: string) {
    console.log(functionname);

  }

  next(): void {
    console.log(this.inputString);
    if (this.stage == 2)
      this.stage = 0;
    else
      this.stage++;
  }
}
