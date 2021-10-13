import { Component, Input, OnDestroy, OnInit } from '@angular/core';
import { MatDialog } from '@angular/material/dialog';
import { ActivatedRoute } from '@angular/router';
import { EnoceanI } from '../enocean-i';
import { EnoceanService } from '../services/enocean.service';
import { IRServiceService } from '../services/irservice.service';

@Component({
  selector: 'app-teach-in-dialog',
  templateUrl: './teach-in-dialog.component.html',
  styleUrls: ['./teach-in-dialog.component.css']
})
export class TeachInDialogComponent implements OnInit, OnDestroy {

  inputString: string = "";
  stage = 0;

  irSignals!: string[];
  enOceanTelegrams!: EnoceanI[];

  constructor(public dialog: MatDialog, private activatedRoute: ActivatedRoute, private irService: IRServiceService, private enoceanService: EnoceanService) {
    /*     irService.startListening();
        this.irSignals = irService.getIrSignals();
        irService.receivableSignal.subscribe((irSignal: string) => {
          console.log("new IR signal: " + irSignal);
          this.irSignals.push(irSignal);
        }) */

  }

  ngOnInit(): void {


  }

  ngOnDestroy(): void {
    this.irService.stopListening();
    this.irService.resetIrSignals();
  }

  openEnOceanDialog(functionname: string) {
    console.log(functionname);
  }

  next(): void {
    this.stage++;
    console.log(this.inputString);
    if (this.stage == 2) {
      this.irService.startListening();
      this.irSignals = this.irService.getIrSignals();
      this.irService.receivableSignal.subscribe((irSignal: string) => {
        console.log("new IR signal: " + irSignal);
        this.irSignals.push(irSignal);
      });
    } else {
      this.irService.stopListening();
      this.irService.resetIrSignals();
    }
    if (this.stage == 1) {
      this.enoceanService.startListening();
      this.enOceanTelegrams = this.enoceanService.getEnoceanTelegramArray();
      this.enoceanService.receivableTelegram.subscribe((enOceanTelegram: any) => {
        console.log("New EnOcean telegram: " + enOceanTelegram);
        this.enOceanTelegrams.push(enOceanTelegram);
      })
    } else {
      this.enoceanService.stopListening();
      this.enoceanService.resetEnoceanTelegramArray();
    }
    if (this.stage > 2) {
      this.stage = 0;
    }
  }

}
