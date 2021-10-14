import { Component, OnInit } from '@angular/core';
import { EntryI } from '../entry-i';
import { EntryService } from '../services/entry.service';


@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.scss']
})
export class DashboardComponent implements OnInit {

  entries: EntryI[] = [];

  constructor(private entryService: EntryService) {

  }

  ngOnInit(): void {
    this.entryService.getEntries().then((entries) => {
      console.log("Got array of entries:");
      console.log(entries);
      this.entries = entries;
    }).catch((error) => console.error(error));
  }

  send(entry: EntryI): void {
    console.log(entry);
    // this.entryService.testIR(entry.entryId);
  }

}


/**
 * TODOS:
 * + Implement API functions.
 * + TEST Sending IR (Physical).
 * + edit Teach in.
 * + Work on UI (DASHBOARD).
 * + FIX NAVIGATION (mobile).
 * + getEntries should get the Entries From RAM (!SPIFFS).
 * +
 */