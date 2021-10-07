import { Component, Input, OnInit } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';
import { DataServiceService } from '../services/data-service.service';
import { EnoceanTableComponent } from '../enocean-table/enocean-table.component';
import { ActivatedRoute } from '@angular/router';

@Component({
  selector: 'app-teach-in-dialog',
  templateUrl: './teach-in-dialog.component.html',
  styleUrls: ['./teach-in-dialog.component.css']
})
export class TeachInDialogComponent implements OnInit {

  inputString: string = "";
  stage = 0;

  @Input() packetType: string = "";
  @Input() senderId: string = "";

  constructor(public dialog: MatDialog, private activatedRoute: ActivatedRoute) {

  }

  // enOceandataMAP = new Map(enoceanDataService.getENOCEANDATA());

  ngOnInit(): void {
    this.activatedRoute.params.subscribe(params => {
      this.packetType = params.type;
      this.senderId = params.senderid;
    });
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

  // printSignals() {
  //   console.log(this._enoceanService.getTest());
  // }

}
