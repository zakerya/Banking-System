import os
import sys
import subprocess
import platform
import shlex
import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox

# Modern dark color scheme
PRIMARY_COLOR = "#4f46e5"        # Indigo for primary actions
SECONDARY_COLOR = "#64748b"      # Slate gray
BACKGROUND_COLOR = "#1e293b"     # Very dark blue (almost black)
CARD_COLOR = "#1e293b"           # Dark blue-gray for cards/containers
TEXT_COLOR = "#e2e8f0"           # Light gray for text
ACCENT_COLOR = "#60a5fa"         # Light blue for highlights
SUCCESS_COLOR = "#10b981"        # Green for positive actions
ERROR_COLOR = "#ef4444"          # Red for errors

# Fonts
HEADING_FONT = ("Segoe UI", 16, "bold")
SUBHEADING_FONT = ("Segoe UI", 12, "bold")
BODY_FONT = ("Segoe UI", 10)
BUTTON_FONT = ("Segoe UI", 10, "bold")

def exe_name():
    base = "bank"
    if platform.system() == "Windows":
        return base + ".exe"
    return "./" + base

BACKEND = exe_name()

def run_backend(args):
    """Run backend executable (list of args, not including program name). Return stdout+stderr."""
    cmd = [BACKEND] + args
    try:
        # use text mode
        completed = subprocess.run(cmd, capture_output=True, text=True, shell=False)
        out = completed.stdout.strip()
        err = completed.stderr.strip()
        if err:
            return err if not out else out + "\n" + err
        return out if out else "(no output)"
    except FileNotFoundError:
        return f"ERROR: Backend executable not found: {BACKEND}\nCompile the C program first."

# GUI callbacks
def create_account():
    pin = ent_create_pin.get().strip()
    name = ent_create_name.get().strip()
    if not pin.isdigit() or not name:
        messagebox.showerror("Input error", "Enter numeric PIN and non-empty name.")
        return
    out = run_backend(["create", pin, name])
    txt_output.delete(1.0, tk.END)
    txt_output.insert(tk.END, out)

def display_accounts():
    out = run_backend(["display"])
    txt_output.delete(1.0, tk.END)
    txt_output.insert(tk.END, out)

def deposit():
    acc = ent_deposit_acc.get().strip()
    pin = ent_deposit_pin.get().strip()
    amt = ent_deposit_amt.get().strip()
    if not acc.isdigit() or not pin.isdigit():
        messagebox.showerror("Input error", "Account and PIN must be numbers.")
        return
    try:
        float(amt)
    except:
        messagebox.showerror("Input error", "Amount must be a valid number.")
        return
    out = run_backend(["deposit", acc, pin, amt])
    txt_output.delete(1.0, tk.END)
    txt_output.insert(tk.END, out)

def withdraw():
    acc = ent_withdraw_acc.get().strip()
    pin = ent_withdraw_pin.get().strip()
    amt = ent_withdraw_amt.get().strip()
    if not acc.isdigit() or not pin.isdigit():
        messagebox.showerror("Input error", "Account and PIN must be numbers.")
        return
    try:
        float(amt)
    except:
        messagebox.showerror("Input error", "Amount must be a valid number.")
        return
    out = run_backend(["withdraw", acc, pin, amt])
    txt_output.delete(1.0, tk.END)
    txt_output.insert(tk.END, out)

def check_balance():
    acc = ent_check_acc.get().strip()
    pin = ent_check_pin.get().strip()
    if not acc.isdigit() or not pin.isdigit():
        messagebox.showerror("Input error", "Account and PIN must be numbers.")
        return
    out = run_backend(["check", acc, pin])
    txt_output.delete(1.0, tk.END)
    txt_output.insert(tk.END, out)

def delete_account():
    acc = ent_delete_acc.get().strip()
    pin = ent_delete_pin.get().strip()
    if not acc.isdigit() or not pin.isdigit():
        messagebox.showerror("Input error", "Account and PIN must be numbers.")
        return
    if not messagebox.askyesno("Confirm", f"Delete account {acc}?"):
        return
    out = run_backend(["delete", acc, pin])
    txt_output.delete(1.0, tk.END)
    txt_output.insert(tk.END, out)

# Create main window
root = tk.Tk()
root.title("Bank Management System")
root.configure(bg=BACKGROUND_COLOR)
root.geometry("900x700")

# Set style
style = ttk.Style()
style.theme_use('clam')

# Configure styles
style.configure("TNotebook", background=BACKGROUND_COLOR, borderwidth=0)
style.configure("TNotebook.Tab", 
                background=BACKGROUND_COLOR, 
                foreground=SECONDARY_COLOR,
                padding=[20, 8],
                font=BUTTON_FONT)
style.map("TNotebook.Tab", 
          background=[("selected", CARD_COLOR)],
          foreground=[("selected", PRIMARY_COLOR)])

style.configure("TFrame", background=BACKGROUND_COLOR)
style.configure("TLabel", background=BACKGROUND_COLOR, foreground=TEXT_COLOR, font=BODY_FONT)
style.configure("Header.TLabel", background=BACKGROUND_COLOR, foreground=TEXT_COLOR, font=HEADING_FONT)
style.configure("Card.TFrame", background=CARD_COLOR, relief="flat", borderwidth=0)

style.configure("Action.TButton", 
                background=PRIMARY_COLOR,
                foreground="white",
                borderwidth=0,
                focusthickness=0,
                focuscolor=PRIMARY_COLOR,
                font=BUTTON_FONT,
                padding=(20, 10))
style.map("Action.TButton", 
          background=[('active', ACCENT_COLOR)])

style.configure("Secondary.TButton", 
                background=SECONDARY_COLOR,
                foreground="white",
                borderwidth=0,
                focusthickness=0,
                focuscolor=SECONDARY_COLOR,
                font=BUTTON_FONT,
                padding=(20, 10))
style.map("Secondary.TButton", 
          background=[('active', TEXT_COLOR)])

style.configure("TEntry", 
                fieldbackground=CARD_COLOR,
                foreground=TEXT_COLOR,
                borderwidth=1,
                lightcolor=PRIMARY_COLOR,
                darkcolor=PRIMARY_COLOR,
                padding=8)
style.map("TEntry", 
          fieldbackground=[('focus', CARD_COLOR)],
          bordercolor=[('focus', PRIMARY_COLOR)])

# Header
header_frame = ttk.Frame(root, style="Card.TFrame")
header_frame.pack(fill="x", padx=20, pady=(20, 0))

ttk.Label(header_frame, text="Bank Management System", style="Header.TLabel").pack(side="left", padx=20, pady=15)

# Main content area
main_frame = ttk.Frame(root)
main_frame.pack(fill="both", expand=True, padx=20, pady=20)

# Create notebook with tabs
notebook = ttk.Notebook(main_frame)
notebook.pack(fill="both", expand=True)

# Create tab frames with consistent padding
tab_frames = {}
for tab_name in ["Create Account", "View Accounts", "Deposit", "Withdraw", "Check Balance", "Delete Account"]:
    frame = ttk.Frame(notebook, padding=20, style="Card.TFrame")
    tab_frames[tab_name] = frame
    notebook.add(frame, text=tab_name)

# Create Account Tab
create_frame = tab_frames["Create Account"]
ttk.Label(create_frame, text="Create New Account", font=SUBHEADING_FONT, foreground=TEXT_COLOR).grid(row=0, column=0, columnspan=2, sticky="w", pady=(0, 20))

ttk.Label(create_frame, text="Account PIN:").grid(row=1, column=0, sticky="e", pady=10, padx=(0, 10))
ent_create_pin = ttk.Entry(create_frame, width=25, font=BODY_FONT)
ent_create_pin.grid(row=1, column=1, sticky="w", pady=10)

ttk.Label(create_frame, text="Account Holder Name:").grid(row=2, column=0, sticky="e", pady=10, padx=(0, 10))
ent_create_name = ttk.Entry(create_frame, width=25, font=BODY_FONT)
ent_create_name.grid(row=2, column=1, sticky="w", pady=10)

btn_create = ttk.Button(create_frame, text="Create Account", command=create_account, style="Action.TButton")
btn_create.grid(row=3, column=0, columnspan=2, pady=20)

# View Accounts Tab
view_frame = tab_frames["View Accounts"]
ttk.Label(view_frame, text="All Accounts", font=SUBHEADING_FONT, foreground=TEXT_COLOR).pack(anchor="w", pady=(0, 20))

btn_display = ttk.Button(view_frame, text="Refresh Accounts", command=display_accounts, style="Action.TButton")
btn_display.pack(pady=10)

# Deposit Tab
deposit_frame = tab_frames["Deposit"]
ttk.Label(deposit_frame, text="Deposit Funds", font=SUBHEADING_FONT, foreground=TEXT_COLOR).grid(row=0, column=0, columnspan=2, sticky="w", pady=(0, 20))

ttk.Label(deposit_frame, text="Account Number:").grid(row=1, column=0, sticky="e", pady=10, padx=(0, 10))
ent_deposit_acc = ttk.Entry(deposit_frame, width=25, font=BODY_FONT)
ent_deposit_acc.grid(row=1, column=1, sticky="w", pady=10)

ttk.Label(deposit_frame, text="PIN:").grid(row=2, column=0, sticky="e", pady=10, padx=(0, 10))
ent_deposit_pin = ttk.Entry(deposit_frame, width=25, font=BODY_FONT, show="•")
ent_deposit_pin.grid(row=2, column=1, sticky="w", pady=10)

ttk.Label(deposit_frame, text="Amount:").grid(row=3, column=0, sticky="e", pady=10, padx=(0, 10))
ent_deposit_amt = ttk.Entry(deposit_frame, width=25, font=BODY_FONT)
ent_deposit_amt.grid(row=3, column=1, sticky="w", pady=10)

btn_deposit = ttk.Button(deposit_frame, text="Deposit", command=deposit, style="Action.TButton")
btn_deposit.grid(row=4, column=0, columnspan=2, pady=20)

# Withdraw Tab
withdraw_frame = tab_frames["Withdraw"]
ttk.Label(withdraw_frame, text="Withdraw Funds", font=SUBHEADING_FONT, foreground=TEXT_COLOR).grid(row=0, column=0, columnspan=2, sticky="w", pady=(0, 20))

ttk.Label(withdraw_frame, text="Account Number:").grid(row=1, column=0, sticky="e", pady=10, padx=(0, 10))
ent_withdraw_acc = ttk.Entry(withdraw_frame, width=25, font=BODY_FONT)
ent_withdraw_acc.grid(row=1, column=1, sticky="w", pady=10)

ttk.Label(withdraw_frame, text="PIN:").grid(row=2, column=0, sticky="e", pady=10, padx=(0, 10))
ent_withdraw_pin = ttk.Entry(withdraw_frame, width=25, font=BODY_FONT, show="•")
ent_withdraw_pin.grid(row=2, column=1, sticky="w", pady=10)

ttk.Label(withdraw_frame, text="Amount:").grid(row=3, column=0, sticky="e", pady=10, padx=(0, 10))
ent_withdraw_amt = ttk.Entry(withdraw_frame, width=25, font=BODY_FONT)
ent_withdraw_amt.grid(row=3, column=1, sticky="w", pady=10)

btn_withdraw = ttk.Button(withdraw_frame, text="Withdraw", command=withdraw, style="Action.TButton")
btn_withdraw.grid(row=4, column=0, columnspan=2, pady=20)

# Check Balance Tab
check_frame = tab_frames["Check Balance"]
ttk.Label(check_frame, text="Check Account Balance", font=SUBHEADING_FONT, foreground=TEXT_COLOR).grid(row=0, column=0, columnspan=2, sticky="w", pady=(0, 20))

ttk.Label(check_frame, text="Account Number:").grid(row=1, column=0, sticky="e", pady=10, padx=(0, 10))
ent_check_acc = ttk.Entry(check_frame, width=25, font=BODY_FONT)
ent_check_acc.grid(row=1, column=1, sticky="w", pady=10)

ttk.Label(check_frame, text="PIN:").grid(row=2, column=0, sticky="e", pady=10, padx=(0, 10))
ent_check_pin = ttk.Entry(check_frame, width=25, font=BODY_FONT, show="•")
ent_check_pin.grid(row=2, column=1, sticky="w", pady=10)

btn_check = ttk.Button(check_frame, text="Check Balance", command=check_balance, style="Action.TButton")
btn_check.grid(row=3, column=0, columnspan=2, pady=20)

# Delete Account Tab
delete_frame = tab_frames["Delete Account"]
ttk.Label(delete_frame, text="Delete Account", font=SUBHEADING_FONT, foreground=TEXT_COLOR).grid(row=0, column=0, columnspan=2, sticky="w", pady=(0, 20))

ttk.Label(delete_frame, text="Account Number:").grid(row=1, column=0, sticky="e", pady=10, padx=(0, 10))
ent_delete_acc = ttk.Entry(delete_frame, width=25, font=BODY_FONT)
ent_delete_acc.grid(row=1, column=1, sticky="w", pady=10)

ttk.Label(delete_frame, text="PIN:").grid(row=2, column=0, sticky="e", pady=10, padx=(0, 10))
ent_delete_pin = ttk.Entry(delete_frame, width=25, font=BODY_FONT, show="•")
ent_delete_pin.grid(row=2, column=1, sticky="w", pady=10)

btn_delete = ttk.Button(delete_frame, text="Delete Account", command=delete_account, style="Action.TButton")
btn_delete.grid(row=3, column=0, columnspan=2, pady=20)

# Output area
output_frame = ttk.Frame(root, style="Card.TFrame")
output_frame.pack(fill="x", padx=20, pady=(0, 20))

ttk.Label(output_frame, text="Output", font=SUBHEADING_FONT, foreground=TEXT_COLOR).pack(anchor="w", padx=20, pady=(15, 10))

txt_output = scrolledtext.ScrolledText(
    output_frame, 
    width=80, 
    height=12,
    bg=CARD_COLOR,
    fg=TEXT_COLOR,
    insertbackground=TEXT_COLOR,
    selectbackground=ACCENT_COLOR,
    font=("Consolas", 9),
    relief="flat",
    borderwidth=1,
    padx=15,
    pady=15
)
txt_output.pack(fill="both", expand=True, padx=20, pady=(0, 20))

# Status bar
status_bar = ttk.Frame(root, style="Card.TFrame")
status_bar.pack(side="bottom", fill="x", padx=20, pady=(0, 10))
ttk.Label(status_bar, text="Ready", style="TLabel").pack(side="left", padx=20, pady=8)

# Start with display
display_accounts()

# Center the window on screen
root.update_idletasks()
width = root.winfo_width()
height = root.winfo_height()
x = (root.winfo_screenwidth() // 2) - (width // 2)
y = (root.winfo_screenheight() // 2) - (height // 2)
root.geometry(f'{width}x{height}+{x}+{y}')

root.mainloop()