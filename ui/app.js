(() => {
 const $=s=>document.querySelector(s), $$=s=>Array.from(document.querySelectorAll(s));
 const dialog=$("#module-dialog"),toast=$("#toast");let toastTimer;
 const modules={
  notifications:["Notifications","There are no connected system services yet. This panel is reserved for verified NUN events and service messages."],
  profile:["Local user","A local development profile for the NUN interface preview."],
  workbench:["Workbench","A dedicated engineering workspace is planned for logs, diagnostic tools and development utilities. Command execution is not connected."],
  files:["Storage","The storage browser will expose mounted volumes and files after a filesystem service is implemented."],
  "architecture-details":["System architecture","The architecture map separates the kernel substrate, bootstrap handoff, device boundary, memory management, execution model and diagnostics."],
  kernel:["Kernel substrate","The kernel is still a specification scaffold. Its future responsibilities include CPU entry, memory primitives, execution context and protected device boundaries."],
  boot:["Bootstrap handoff","The bootstrap layer will validate an image, check platform capabilities, build a versioned boot-information block and transfer control through a defined entry point."],
  drivers:["Device boundary","Device drivers will connect through documented bus and driver interfaces. No hardware discovery or driver runtime is present in this prototype."],
  "components-catalog":["Component catalog","A structured catalog of planned NUN components. Each component will have an owner, contract, implementation state and verification status."],
  memory:["Memory manager","Planned memory work includes physical page tracking, an early allocator, virtual address mapping and access protection."],
  execution:["Execution model","The execution layer will define task context, scheduling boundaries and explicit synchronization rules."],
  diagnostics:["Diagnostics","The diagnostic layer is planned to provide early serial or framebuffer output and structured fault records."],
  "activity-log":["Workspace log","This timeline shows illustrative project events only. It is not a live system journal."]
 };
 function notify(message){toast.textContent=message;toast.classList.add("show");clearTimeout(toastTimer);toastTimer=setTimeout(()=>toast.classList.remove("show"),2200)}
 function openModule(key){const data=modules[key]||[key,"This section is a conceptual part of the NUN environment."];$("#dialog-title").textContent=data[0];$("#dialog-copy").textContent=data[1];if(dialog.showModal)dialog.showModal();else notify(data[0]+" · preview")}
 $$("[data-open]").forEach(button=>button.addEventListener("click",()=>openModule(button.dataset.open)));
 $("#dialog-close").addEventListener("click",()=>dialog.close());$("#dialog-action").addEventListener("click",()=>{dialog.close();notify("Panel closed")});dialog.addEventListener("click",event=>{if(event.target===dialog)dialog.close()});
 const pageText={
  Overview:["System, in <em>balance.</em>","A clear view of the environment taking shape."],
  Workbench:["Make room to <em>build.</em>","Engineering tools will live here as the runtime evolves."],
  Architecture:["Every layer has a <em>boundary.</em>","Explore the contracts that define the NUN system."],
  Components:["Small parts. <em>One system.</em>","A catalog of planned modules and their responsibilities."],
  Storage:["A place for <em>everything.</em>","Storage tools will appear when the filesystem layer exists."],
  Settings:["Set your <em>environment.</em>","Interface preferences and accessibility controls."]
 };
 $$(".side-link[data-page]").forEach(button=>button.addEventListener("click",()=>{const page=button.dataset.page;$$(".side-link").forEach(item=>item.classList.toggle("active",item===button));$("#breadcrumb-current").textContent=page.toUpperCase();$("#page-title").innerHTML=pageText[page][0];$("#page-subtitle").textContent=pageText[page][1];notify(page+" view selected")}));
 $("#global-search").addEventListener("input",event=>{const query=event.target.value.trim().toLowerCase();let shown=0;$$(".component-row").forEach(row=>{const match=!query||row.innerText.toLowerCase().includes(query);row.hidden=!match;if(match)shown++});if(query)notify(shown?shown+" matching components":"No matching components")});
 function updateClock(){const now=new Date();$("#clock").textContent=new Intl.DateTimeFormat("en-GB",{hour:"2-digit",minute:"2-digit"}).format(now)}
 $("#date-chip").textContent=new Intl.DateTimeFormat("en-GB",{weekday:"short",month:"short",day:"2-digit"}).format(new Date()).toUpperCase();
 updateClock();setInterval(updateClock,15000);
 document.addEventListener("keydown",event=>{if((event.metaKey||event.ctrlKey)&&event.key.toLowerCase()==="k"){event.preventDefault();$("#global-search").focus()}});
})();