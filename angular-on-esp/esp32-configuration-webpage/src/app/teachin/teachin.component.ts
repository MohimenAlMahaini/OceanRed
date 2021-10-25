import { Component, OnDestroy, OnInit } from '@angular/core';
import { MatDialog } from '@angular/material/dialog'
import { EnoceanI } from '../enocean-i';
import { EntryI } from '../entry-i';
import { EnoceanService } from '../services/enocean.service';
import { EntryService } from '../services/entry.service';
import { TeachInDialogComponent } from '../teach-in-dialog/teach-in-dialog.component';
import { TeachinEnOceanComponent } from '../teachin-en-ocean/teachin-en-ocean.component';

@Component({
  selector: 'app-teachin',
  templateUrl: './teachin.component.html',
  styleUrls: ['./teachin.component.scss']
})
export class TeachinComponent implements OnInit
{

  entries: EntryI[] = [];
  enoceanArray: string[] = [];

  constructor(public dialog: MatDialog, private entryService: EntryService) { }

  ngOnInit(): void
  {
    this.refreshEntries();
    // this.initMockdata();
  }

  parseEnocean(enoceanStr: string): string[]
  {
    return enoceanStr.split('#');
  }

  openDialog()
  {
    this.dialog.open(TeachInDialogComponent);
  }

  removeEnocean(entryId: number, enoceanID: string)
  {
    this.entryService.removeEnocean(entryId.toString(), enoceanID).then((result: boolean) => this.refreshEntries()).catch((error) => console.error(error))
  }

  addEnocean(entryId: number)
  {
    this.entryService.currentID = entryId;
    this.dialog.open(TeachinEnOceanComponent);
  }

  saveEntry()
  {
    this.entryService.save();
  }

  removeEntry(entryId: number)
  {
    this.entryService.removeEntry(entryId.toString());
  }

  refreshEntries(): void
  {
    this.entryService.getEntries().then((entries) =>
    {
      console.log("Got array of entries:");
      console.log(entries);
      this.entries = entries;
    }).catch((error) => console.error(error));
  }

  restartESP()
  {
    this.entryService.restart();
  }

}

